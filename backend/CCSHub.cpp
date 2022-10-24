#include "CCSHub.hpp"

CCSHub::CCSHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_CCS811()),
    eco2(Sensor("CCS", "co2", "eCO2", new WindowedReading<float, SAMPLE_BACKLOG>())),
    voc(Sensor("CCS", "voc", "VOC", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void CCSHub::begin() {
  this->sensor.begin(this->address, this->i2c);

  // FIXME Do this based on HTU readings.
  this->sensor.setEnvironmentalData(40, 25);
}

void CCSHub::update() {
  if(this->sensor.available() && !this->sensor.readData()) {
    this->eco2.add(this->sensor.geteCO2());
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
