#include "MHZHub.hpp"

MHZHub::MHZHub(uint8_t rx, uint8_t tx):
    sensor(MHZ19()),
    co2(Sensor("MHZ", "co2", "CO2", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("MHZ", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void MHZHub::begin() {
  this->serial->begin(MHZ_BAUDRATE);
  this->sensor.begin(*(this->serial));
  this->sensor.autoCalibration(false);
  this->sensor.calibrate();
}

void MHZHub::update() {
  float co2 = this->sensor.getCO2();
  if(this->sensor.errorCode == RESULT_OK) {
    this->co2.add(co2);
  } else {
    this->co2.add(NAN);
  }

  float temp = this->sensor.getTemperature();
  if(this->sensor.errorCode == RESULT_OK) {
    this->temperature.add(temp);
  } else {
    this->temperature.add(NAN);
  }
}

void MHZHub::connect(Device *d) const {
  d->attach(&this->co2);
  d->attach(&this->temperature);
}
