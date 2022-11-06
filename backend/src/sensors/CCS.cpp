#include "CCS.hpp"

CCS::CCS(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(CCS811(addr)),
    eco2(Reading<float>("CO2", "CCS", "co2", new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 29206))),
    voc(Reading<float>("Total VOC", "CCS", "voc", new WindowedValue<float, SAMPLE_BACKLOG>("ppb", 0, 32768)))
{}

void CCS::initSensor() {
  this->sensor.begin(*(this->i2c));

  this->eco2.setStatus("init");
  this->voc.setStatus("init");

  uint32_t t0 = millis();
  uint32_t t1 = t0;
  while(!this->sensor.dataAvailable(), (t1 - t0) < CCS_INIT_TIME) {
    delay(10);
    t1 = millis();
  }

  if(!this->sensor.dataAvailable()) {
    String error = String("Sensor initialization took more than ") + String(CCS_INIT_TIME) + "ms.";
    this->eco2.setError(error);
    this->voc.setError(error);
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
    this->eco2.setError(error);
    this->voc.setError(error);
    return;
  }

  CCS811::CCS811_Status_e result = this->sensor.readAlgorithmResults();

  if(result == CCS811::CCS811_Stat_SUCCESS) {
    this->eco2.add(this->sensor.getCO2());
    this->voc.add(this->sensor.getTVOC());
  } else {
    String error = String("Could not read sensor. Response:  ") + String(result);
    this->eco2.setError(error);
    this->voc.setError(error);
  }
}

void CCS::connect(Device *d) const {
  d->attach(&this->eco2);
  d->attach(&this->voc);
}

void CCS::reset() {
  this->initSensor();
}

void CCS::setCompensationParameters(float temperature, float humidity) {
  this->sensor.setEnvironmentalData(humidity, temperature);
}