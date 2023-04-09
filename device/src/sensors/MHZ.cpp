#include "MHZ.hpp"

MHZ::MHZ(uint8_t rx, uint8_t tx, uint16_t interval, uint16_t warmup):
    sensor(MHZ19()),
    sampleInterval(interval),
    warmupTime(warmup),
    co2(nullptr),
    temperature(nullptr)
{
  this->serial = new SoftwareSerial(rx, tx);
}

MHZ::~MHZ() {
  if (this->temperature != nullptr) delete this->temperature;
  if (this->co2 != nullptr) delete this->co2;
  if (this->serial != nullptr) delete this->serial;
}

MHZ* MHZ::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  uint16_t warmup = (int) config["warmupTime"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t rx = (int) conn["rx"];
  uint16_t tx = (int) conn["tx"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "software-uart") {
    return nullptr;
  }

  MHZ *mhz = new MHZ(rx, tx, interval, warmup);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "temperature") {
      mhz->temperature = new Reading<float>(name, "MHZ", type, new WindowedValue<float>(window, "°C", 0, 100), cfg);
    } else if (type == "co2") {
      mhz->co2 = new Reading<float>(name, "MHZ", type, new WindowedValue<float>(window, "ppm", 0, 10000), cfg);
    }
  }

  return mhz;
}

void MHZ::initSensor() {
  this->sensor.begin(*(this->serial));
  this->sensor.autoCalibration(false);
  this->sensor.calibrate();
}

void MHZ::begin(System &system) {
  this->serial->begin(MHZ_BAUDRATE);
  this->initSensor();

  system.device().attach(this);

  system.scheduler().spawn("sample MHZ", 115,[&](Task *t) {
    system.log().debug("Sampling MHZ sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });

  system.scheduler().spawn("reset MHZ", 125,[&](Task *t) {
    static boolean mhzWarmup = true;

    if (mhzWarmup) {
      mhzWarmup = false;
      t->sleep(this->warmupTime);
    } else {
      system.log().info("Resetting MHZ sensor after a warmup.");
      this->reset();
      t->kill();
    }
  });
}

void MHZ::update() {
  if (this->co2 != nullptr) {
    float co2 = this->sensor.getCO2();
    if(this->sensor.errorCode == RESULT_OK) {
      this->co2->add(co2);
    } else {
      this->co2->setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
    }
  }

  if (this->temperature != nullptr) {
    float temp = this->sensor.getTemperature();
    if(this->sensor.errorCode == RESULT_OK) {
      this->temperature->add(temp);
    } else {
      this->co2->setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
    }
  }
}

void MHZ::connect(Device *d) const {
  if (this->co2 != nullptr) d->attach(this->co2);
  if (this->temperature != nullptr) d->attach(this->temperature);
}

void MHZ::reset() {
  this->initSensor();
}
