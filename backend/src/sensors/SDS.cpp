#include "SDS.hpp"

PatchedSdsSensor::PatchedSdsSensor(HardwareSerial &serial):
  SdsDustSensor(serial, RETRY_DELAY_MS_DEFAULT, MAX_RETRIES_NOT_AVAILABLE_DEFAULT),
  serial(serial)
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

SDS::SDS(HardwareSerial &serial):
    serial(serial),
    sensor(PatchedSdsSensor(serial)),
    pm25(nullptr),
    pm10(nullptr)
{}

SDS::~SDS() {
  if (this->pm25 != nullptr) delete this->pm25;
  if (this->pm10 != nullptr) delete this->pm10;
}

SDS* SDS::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t number = (int) conn["number"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-uart") {
    return nullptr;
  }

  SDS *sds;

  switch(number) {
#ifdef ESP32
    case 2: {
      HardwareSerial& sdsSerial(Serial2);
      sds = new SDS(sdsSerial);
    }
      break;
#endif

    case 1: {
      HardwareSerial& sdsSerial(Serial1);
      sds = new SDS(sdsSerial);
    }
      break;

    case 0:
    default: {
      HardwareSerial& sdsSerial(Serial);
      sds = new SDS(sdsSerial);
    }
      break;
  }

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "pm10") {
      sds->pm10 = new Reading<float>(name, "SDS", type, new WindowedValue<float, SAMPLE_BACKLOG>("μg/m³", 0, 1000), cfg);
    } else if (type == "pm2.5") {
      sds->pm25 = new Reading<float>(name, "SDS", type, new WindowedValue<float, SAMPLE_BACKLOG>("μg/m³", 0, 1000), cfg);
    }
  }

  return sds;
}

void SDS::begin(System &system) {
  this->sensor.begin();
  this->sensor.setQueryReportingMode();

  system.device().attach(this);

  system.scheduler().spawn("sample SDS", 115,[&](Task *t) {
    system.log().debug("Sampling SDS sensor.");
    this->update();
    t->sleep(SDS_SAMPLE_INTERVAL);
  });
}

void SDS::update() {
  this->sensor.pollPm();
  PmResult pm = this->sensor.readPm();
  if (pm.isOk()) {
    if (this->pm25 != nullptr)  this->pm25->add(pm.pm25);
    if (this->pm10 != nullptr)  this->pm10->add(pm.pm10);
  } else {
    String error = "Could not read sensor.";
    if (this->pm25 != nullptr)  this->pm25->setError(error);
    if (this->pm10 != nullptr)  this->pm10->setError(error);
  }
}

void SDS::connect(Device *d) const {
  if (this->pm25 != nullptr)  d->attach(this->pm25);
  if (this->pm10 != nullptr)  d->attach(this->pm10);
}
