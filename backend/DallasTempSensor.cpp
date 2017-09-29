#include "DallasTempSensor.hpp"

DallasTempSensor::DallasTempSensor(uint8_t pin, uint8_t resolution):
    oneWire(OneWire(pin)),
    sensors(DallasTemperature(&this->oneWire))
{
  this->sensors.setResolution(resolution);
  this->sensors.setWaitForConversion(false);
  this->sStatus = "error";
}

DallasTempSensor::~DallasTempSensor() {
  if(this->temperatures != nullptr) {
    delete this->temperatures;
  }
}

void DallasTempSensor::begin() {
  this->sensors.begin();
  this->nSensors = this->sensors.getDeviceCount();
  this->temperatures = new WindowedReading<float, SAMPLE_BACKLOG>[this->nSensors];
  this->sensors.requestTemperatures();
}

void DallasTempSensor::update() {
  for(uint8_t i = 0; i < this->nSensors; ++i) {
    float temp = this->sensors.getTempCByIndex(i);

    if(!isnan(temp) && temp != DEVICE_DISCONNECTED_C && temp != DEVICE_DISCONNECTED_RAW) {
      this->temperatures[i].add(temp);
      this->nMeasurements++;
      this->sStatus = "ok";
    } else {
      this->sStatus = "error";
      this->nErrors++;
    }
  }

  this->sensors.requestTemperatures();
}

String DallasTempSensor::type() const {
  return "DallasTemperature";
}

String DallasTempSensor::toJSON() const {
  String json = "{";
  json += "\"type\":\"" + this->type() + "\"";
  json += ",\"status\":\"" + this->status() + "\"";
  json += ",\"errors\":" + String(this->errors());
  json += ",\"measurements\":" + String(this->measurements());

  for(uint8_t i = 0; i < this->nSensors; ++i) {
    json += ",\"temperature" + String(i) + "\":" + this->temperatures[i].toJSON();
  }

  json += "}";
  return json;
}
