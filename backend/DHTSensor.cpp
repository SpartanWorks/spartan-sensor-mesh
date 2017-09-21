#include "DHTSensor.hpp"

DHTSensor::DHTSensor(uint16_t pin, uint16_t model):
    sensor(DHT(pin, model)),
    humidity(SmoothMetric<float, SAMPLE_BACKLOG>()),
    temperature(SmoothMetric<float, SAMPLE_BACKLOG>())
{}

void DHTSensor::begin() {
  this->sensor.begin();
}

void DHTSensor::update() {
  float hum = this->sensor.readHumidity(true);
  float temp = this->sensor.readTemperature(false, true);

  if(!isnan(hum) && !isnan(temp)) {
    this->humidity.add(hum);
    this->temperature.add(temp);
    this->nMeasurements++;
    this->error = false;
  } else {
    this->nErrors++;
    this->error = true;
  }
}

String DHTSensor::type() const {
  return "DHT";
}

String DHTSensor::status() const {
  return this->error ? "error" : "ok";
}

uint32_t DHTSensor::errors() const {
  return this->nErrors;
}

uint32_t DHTSensor::measurements() const {
  return this->nMeasurements;
}

String DHTSensor::toJSON() const {
  String json = "{";
  json += "\"type\":\"" + this->type() + "\"";
  json += ",\"status\":\"" + this->status() + "\"";
  json += ",\"errors\":" + String(this->errors());
  json += ",\"measurements\":" + String(this->measurements());
  json += ",\"humidity\":" + this->humidity.toJSON();
  json += ",\"temperature\":" + this->temperature.toJSON();
  json += "}";
  return json;
}
