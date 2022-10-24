#include "CCSHub.hpp"

CCSHub::CCSHub(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(Adafruit_CCS811()),
    eco2(Sensor("CCS", "co2", "eCO2", new WindowedReading<float, SAMPLE_BACKLOG>())),
    voc(Sensor("CCS", "voc", "VOC", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("CCS", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void CCSHub::begin() {
  Wire.begin(this->sda, this->scl);
  this->sensor.begin(this->address);

  // FIXME Do this based on HTU readings.
  this->sensor.setEnvironmentalData(40, 25);
  this->sensor.setTempOffset(85);
}

void CCSHub::update() {
  if(this->sensor.available() && !this->sensor.readData()) {
    this->eco2.add(this->sensor.geteCO2());
    this->voc.add(this->sensor.getTVOC());
    this->temperature.add(this->sensor.calculateTemperature());
  } else {
    this->eco2.add(NAN);
    this->voc.add(NAN);
    this->temperature.add(NAN);
  }
}

void CCSHub::connect(Device *d) const {
  d->attach(&this->eco2);
  d->attach(&this->voc);
  d->attach(&this->temperature);
}
