#include "DHTSensor.hpp"

DHTSensor::DHTSensor(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(WindowedReading<float, SAMPLE_BACKLOG>()),
    temperature(WindowedReading<float, SAMPLE_BACKLOG>())
{
  this->sStatus = "error";
  this->sType = "DHT";
}

void DHTSensor::begin() {
  this->sensor.begin();
}

void DHTSensor::update() {
  float hum = this->sensor.readHumidity();
  float temp = this->sensor.readTemperature();

  if(!isnan(hum) && !isnan(temp)) {
    this->humidity.add(hum);
    this->temperature.add(temp);
    this->nMeasurements++;
    this->sStatus = "ok";
  } else {
    this->nErrors++;
    this->sStatus = "error";
  }
}

String DHTSensor::toJSON() const {
  String json = "{";
  json += "\"type\":\"" + this->type() + "\"";
  json += ",\"status\":\"" + this->status() + "\"";
  json += ",\"errors\":" + String(this->errors());
  json += ",\"measurements\":" + String(this->measurements());
  json += ",\"readings\":{";
  json += "\"humidity\":" + this->humidity.toJSON();
  json += ",\"temperature\":" + this->temperature.toJSON();
  json += "}}";
  return json;
}
