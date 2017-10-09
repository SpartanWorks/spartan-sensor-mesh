#include "Sensor.hpp"

Sensor::Sensor(String model, String type, String name, Reading<float> *reading):
    sModel(model), sType(type), sName(name), sReading(reading)
{}

Sensor::~Sensor() {
  if (this->sReading != nullptr) {
    delete this->sReading;
  }
}

String Sensor::model() const {
  return sModel;
}

String Sensor::type() const {
  return sType;
}

String Sensor::name() const {
  return sName;
}

String Sensor::status() const {
  return sStatus;
}

uint32_t Sensor::errors() const {
  return nErrors;
}

uint32_t Sensor::measurements() const {
  return nMeasurements;
}

const Reading<float>* Sensor::reading() const {
  return sReading;
}

void Sensor::add(float s) {
  if (!isnan(s)) {
    this->sReading->add(s);
    this->sStatus = "ok";
    this->nMeasurements++;
  } else {
    this->sStatus = "error";
    this->nErrors++;
  }
}

String Sensor::toJSON() const {
  String json = "{";
  json += "\"model\":\"" + this->model() + "\"";
  json += ",\"type\":\"" + this->type() + "\"";
  json += ",\"name\":\"" + this->name() + "\"";
  json += ",\"status\":\"" + this->status() + "\"";
  json += ",\"errors\":" + String(this->errors());
  json += ",\"measurements\":" + String(this->measurements());
  json += ",\"reading\":" + this->reading()->toJSON();
  json += "}";
  return json;
}
