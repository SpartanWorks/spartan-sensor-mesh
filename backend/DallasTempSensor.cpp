#include "DallasTempSensor.hpp"

DallasTempSensor::DallasTempSensor(uint8_t pin, uint8_t resolution):
    oneWire(OneWire(pin)),
    sensor(DallasTemperature(&this->oneWire)),
    temperature(Reading<float>())
{
  this->sensor.setResolution(resolution);
  this->sensor.setWaitForConversion(false);
}

void DallasTempSensor::begin() {
  this->sensor.begin();
  this->sensor.requestTemperatures();
}

void DallasTempSensor::update() {
  float temp = this->sensor.getTempCByIndex(0);

  if(!isnan(temp) && temp != DEVICE_DISCONNECTED_C && temp != DEVICE_DISCONNECTED_RAW) {
    this->temperature.add(temp);
    this->nMeasurements++;
    this->sStatus = "ok";
  } else {
    this->nErrors++;
    this->sStatus = "error";
  }

  this->sensor.requestTemperatures();
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
  json += ",\"temperature\":" + this->temperature.toJSON();
  json += "}";
  return json;
}
