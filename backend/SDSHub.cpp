#include "SDSHub.hpp"

SDSHub::SDSHub(HardwareSerial &serial):
    serial(serial),
    sensor(SdsDustSensor(serial)),
    pm25(Sensor("SDS", "PM", "PM 2.5", new WindowedReading<float, SAMPLE_BACKLOG>())),
    pm10(Sensor("SDS", "PM", "PM 10", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void SDSHub::begin() {
  this->sensor.begin();
  Serial.println(this->sensor.queryFirmwareVersion().toString());
  this->sensor.setActiveReportingMode();
  this->sensor.setContinuousWorkingPeriod();  
}

void SDSHub::update() {
  PmResult pm = this->sensor.readPm();
  if (pm.isOk()) {
    this->pm25.add(pm.pm25);
    this->pm10.add(pm.pm10);
  } else {
    this->pm25.add(NAN);
    this->pm10.add(NAN);
  }
}

void SDSHub::connect(Device *d) const {
  d->attach(&this->pm25);
  d->attach(&this->pm10);
}
