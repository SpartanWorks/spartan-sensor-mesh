#include "SGP.hpp"

SGP::SGP(TwoWire *i2c, uint8_t addr, uint16_t interval, uint16_t warmup):
    i2c(i2c),
    address(addr),
    sensor(SGP30(i2c)),
    sampleInterval(interval),
    warmupTime(warmup),
    voc(nullptr),
    co2(nullptr),
    h2(nullptr),
    ethanol(nullptr)
{}

SGP::~SGP() {
  if (this->voc != nullptr) delete this->voc;
  if (this->co2 != nullptr) delete this->co2;
  if (this->h2 != nullptr) delete this->h2;
  if (this->ethanol != nullptr) delete this->ethanol;
}

SGP* SGP::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  uint16_t warmup = (int) config["warmupTime"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t address = (int) conn["address"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-i2c") {
    return nullptr;
  }

  SGP *sgp = new SGP(&Wire, address, interval, warmup);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "voc") {
      sgp->voc = new Reading<float>(name, "SGP", type, new WindowedValue<float>(window, "ppb", 0, 60000), cfg);
    } else if (type == "co2") {
      sgp->co2 = new Reading<float>(name, "SGP", type, new WindowedValue<float>(window, "ppm", 0, 57330), cfg);
    } else if (type == "h2") {
      sgp->h2 = new Reading<float>(name, "SGP", type, new WindowedValue<float>(window, "ppm", 0, 1000), cfg);
    } else if (type == "ethanol") {
      sgp->ethanol = new Reading<float>(name, "SGP", type, new WindowedValue<float>(window, "ppm", 0, 1000), cfg);
    }
  }

  return sgp;
}

void SGP::begin(System &system) {
  this->sensor.begin(); // FIXME Use this->address during init.

  uint32_t t0 = millis();
  uint32_t t1 = t0;
  while(!this->sensor.isConnected(), (t1 - t0) < SGP_INIT_TIME) {
    delay(10);
    t1 = millis();
  }

  if(!this->sensor.isConnected()) {
    String error = String("Sensor initialization took more than ") + String(SGP_INIT_TIME) + "ms.";
    if (this->co2 != nullptr)this->co2->setError(error);
    if (this->voc != nullptr)this->voc->setError(error);
    if (this->h2 != nullptr)this->h2->setError(error);
    if (this->ethanol != nullptr)this->ethanol->setError(error);
  } else {
    this->sensor.measureTest();
    this->sensor.GenericReset();
  }

  system.device().attach(this);

  system.scheduler().spawn("sample SGP", 115,[&](Task *t) {
    system.log().debug("Sampling SGP sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });

  system.scheduler().spawn("reset SGP", 125,[&](Task *t) {
    static boolean sgpWarmup = true;

    if (sgpWarmup) {
      sgpWarmup = false;
      t->sleep(this->warmupTime);
    } else {
      system.log().info("Resetting SGP sensor after a warmup.");
      this->sensor.GenericReset();
      t->kill();
    }
  });
}

void SGP::update() {
  if(!this->sensor.isConnected()) {
    String error = "Sensor is not available.";
    if (this->co2 != nullptr) this->co2->setError(error);
    if (this->voc != nullptr) this->voc->setError(error);
    if (this->h2 != nullptr) this->h2->setError(error);
    if (this->ethanol != nullptr) this->ethanol->setError(error);
    return;
  }

  bool result = this->sensor.measure(true);

  if(result) {
    if (this->co2 != nullptr) this->co2->add(this->sensor.getCO2());
    if (this->voc != nullptr) this->voc->add(this->sensor.getTVOC());
    if (this->h2 != nullptr) this->h2->add(this->sensor.getH2());
    if (this->ethanol != nullptr) this->ethanol->add(this->sensor.getEthanol());
  } else {
    String error = String("Could not read sensor:" + String(this->sensor.lastError()));
    if (this->co2 != nullptr) this->co2->setError(error);
    if (this->voc != nullptr) this->voc->setError(error);
    if (this->h2 != nullptr) this->h2->setError(error);
    if (this->ethanol != nullptr) this->ethanol->setError(error);
  }
}

void SGP::connect(Device *d) const {
  if (this->co2 != nullptr) d->attach(this->co2);
  if (this->voc != nullptr) d->attach(this->voc);
  if (this->h2 != nullptr) d->attach(this->h2);
  if (this->ethanol != nullptr) d->attach(this->ethanol);
}

void SGP::setCompensationParameters(float temperature, float humidity) {
  this->sensor.setRelHumidity(temperature, humidity);
}
