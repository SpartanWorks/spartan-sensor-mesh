#include "SDSHub.hpp"

PatchedSdsSensor::PatchedSdsSensor(HardwareSerial &serial):
  serial(serial),
  SdsDustSensor(serial, RETRY_DELAY_MS_DEFAULT, MAX_RETRIES_NOT_AVAILABLE_DEFAULT)
{}

void PatchedSdsSensor::writeImmediate(const Command &command) {
  for (int i = 0; i < Command::length; ++i) {
    serial.write(command.bytes[i]);
  }
}

void PatchedSdsSensor::pollPm() {
  while(serial.available() > 0) {
    serial.read();
  }
  writeImmediate(Commands::queryPm);
}

SDSHub::SDSHub(HardwareSerial &serial):
    serial(serial),
    sensor(PatchedSdsSensor(serial)),
    pm25(Sensor<float>("PM 2.5", "SDS", "pm2.5", new WindowedReading<float, SAMPLE_BACKLOG>("μg/m³", 0, 1000))),
    pm10(Sensor<float>("PM 10", "SDS", "pm10", new WindowedReading<float, SAMPLE_BACKLOG>("μg/m³", 0, 1000)))
{}

void SDSHub::begin() {
  this->sensor.begin();
  Serial.println(this->sensor.queryFirmwareVersion().toString());
  this->sensor.setQueryReportingMode();
}

void SDSHub::update() {
  this->sensor.pollPm();
  PmResult pm = this->sensor.readPm();
  if (pm.isOk()) {
    this->pm25.add(pm.pm25);
    this->pm10.add(pm.pm10);
  } else {
    String error = "Could not read sensor.";
    this->pm25.setError(error);
    this->pm10.setError(error);
  }
}

void SDSHub::connect(Device *d) const {
  d->attach(&this->pm25);
  d->attach(&this->pm10);
}
