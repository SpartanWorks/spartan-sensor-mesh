#include "MHZHub.hpp"

MHZHub::MHZHub(uint8_t rx, uint8_t tx):
    sensor(MHZ19()),
    co2(Sensor<float>("CO2", "MHZ", "co2", new WindowedReading<float, SAMPLE_BACKLOG>("ppm", 0, 10000))),
    temperature(Sensor<float>("temperature", "MHZ", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>("°C", 0, 100)))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void MHZHub::initSensor() {
  this->sensor.begin(*(this->serial));
  this->sensor.autoCalibration(false);
  this->sensor.calibrate();
}

void MHZHub::begin() {
  this->serial->begin(MHZ_BAUDRATE);
  this->initSensor();
}

void MHZHub::update() {
  float co2 = this->sensor.getCO2();
  if(this->sensor.errorCode == RESULT_OK) {
    this->co2.add(co2);
  } else {
    this->co2.setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
  }

  float temp = this->sensor.getTemperature();
  if(this->sensor.errorCode == RESULT_OK) {
    this->temperature.add(temp);
  } else {
    this->co2.setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
  }
}

void MHZHub::connect(Device *d) const {
  d->attach(&this->co2);
  d->attach(&this->temperature);
}

void MHZHub::reset() {
  this->initSensor();
}
