#include "BMPSensor.hpp"

BMPSensor::BMPSensor(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(Adafruit_BMP280())
{
  this->sStatus = "error";
  this->sType = "BMP";
}

void BMPSensor::begin() {
  Wire.begin(this->sda, this->scl);
  this->sensor.begin(this->address);
}

void BMPSensor::update() {
  float press = this->sensor.readPressure();
  float temp = this->sensor.readTemperature();

  if(!isnan(press) && !isnan(temp)) {
    this->pressure.add(press);
    this->temperature.add(temp);
    this->nMeasurements++;
    this->sStatus = "ok";
  } else {
    this->nErrors++;
    this->sStatus = "error";
  }
}

String BMPSensor::toJSON() const {
  String json = "{";
  json += "\"type\":\"" + this->type() + "\"";
  json += ",\"status\":\"" + this->status() + "\"";
  json += ",\"errors\":" + String(this->errors());
  json += ",\"measurements\":" + String(this->measurements());
  json += ",\"readings\":{";
  json += "\"pressure\":" + this->pressure.toJSON();
  json += ",\"temperature\":" + this->temperature.toJSON();
  json += "}}";
  return json;
}
