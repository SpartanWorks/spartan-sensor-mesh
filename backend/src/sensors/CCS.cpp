#include "CCS.hpp"

CCS::CCS(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(CCS811(addr)),
    eco2(nullptr),
    voc(nullptr)
{}

CCS::~CCS() {
  if (this->voc != nullptr) delete this->voc;
  if (this->eco2 != nullptr) delete this->eco2;
}

CCS* CCS::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t address = (int) conn["address"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-i2c") {
    return nullptr;
  }

  CCS *ccs = new CCS(&Wire, address);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "voc") {
      ccs->voc = new Reading<float>(name, "CCS", type, new WindowedValue<float, SAMPLE_BACKLOG>("ppb", 0, 32768), cfg);
    } else if (type == "co2") {
      ccs->eco2 = new Reading<float>(name, "CCS", type, new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 29206), cfg);
    }
  }

  return ccs;
}

void CCS::initSensor() {
  this->sensor.begin(*(this->i2c));

  uint32_t t0 = millis();
  uint32_t t1 = t0;
  while(!this->sensor.dataAvailable(), (t1 - t0) < CCS_INIT_TIME) {
    delay(10);
    t1 = millis();
  }

  if(!this->sensor.dataAvailable()) {
    String error = String("Sensor initialization took more than ") + String(CCS_INIT_TIME) + "ms.";
    if (this->eco2 != nullptr) this->eco2->setError(error);
    if (this->voc != nullptr) this->voc->setError(error);
  }
}

void CCS::begin(System &system) {
  this->initSensor();
  system.device().attach(this);

  system.scheduler().spawn("sample CCS", 115,[&](Task *t) {
    system.log().debug("Sampling CCS sensor.");
    this->update();
    t->sleep(CCS_SAMPLE_INTERVAL);
  });

  system.scheduler().spawn("reset CCS", 125,[&](Task *t) {
    static boolean ccsWarmup = true;

    if (ccsWarmup) {
      ccsWarmup = false;
      t->sleep(CCS_WARMUP_TIMEOUT);
    } else {
      system.log().info("Resetting CCS sensor after a warmup.");
      this->reset();
      t->kill();
    }
  });
}

void CCS::update() {
  if(!this->sensor.dataAvailable()) {
    String error = "Sensor is not available.";
    if (this->eco2 != nullptr) this->eco2->setError(error);
    if (this->voc != nullptr) this->voc->setError(error);
    return;
  }

  CCS811::CCS811_Status_e result = this->sensor.readAlgorithmResults();

  if(result == CCS811::CCS811_Stat_SUCCESS) {
    if (this->eco2 != nullptr) this->eco2->add(this->sensor.getCO2());
    if (this->voc != nullptr) this->voc->add(this->sensor.getTVOC());
  } else {
    String error = String("Could not read sensor. Response:  ") + String(result);
    if (this->eco2 != nullptr) this->eco2->setError(error);
    if (this->voc != nullptr) this->voc->setError(error);
  }
}

void CCS::connect(Device *d) const {
  if (this->eco2 != nullptr) d->attach(this->eco2);
  if (this->voc != nullptr) d->attach(this->voc);
}

void CCS::reset() {
  this->initSensor();
}

void CCS::setCompensationParameters(float temperature, float humidity) {
  this->sensor.setEnvironmentalData(humidity, temperature);
}
