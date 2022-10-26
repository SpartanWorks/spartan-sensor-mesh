#include "CCSHub.hpp"

CCSHub::CCSHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(CCS811(addr)),
    eco2(Sensor("CCS", "co2", "eCO2", new WindowedReading<float, SAMPLE_BACKLOG>())),
    voc(Sensor("CCS", "voc", "VOC", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void CCSHub::initSensor() {
  this->sensor.begin(*(this->i2c));
  this->sensor.setEnvironmentalData(40.0, 26.0);
}

void CCSHub::begin() {
  this->initSensor();
}

void CCSHub::update() {
  if(this->sensor.dataAvailable() && this->sensor.readAlgorithmResults() == CCS811::CCS811_Stat_SUCCESS) {
    this->eco2.add(this->sensor.getCO2());
    this->voc.add(this->sensor.getTVOC());
  } else {
    this->eco2.add(NAN);
    this->voc.add(NAN);
  }
}

void CCSHub::connect(Device *d) const {
  d->attach(&this->eco2);
  d->attach(&this->voc);
}

void CCSHub::reset() {
  this->initSensor();
}
