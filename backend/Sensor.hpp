#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <Arduino.h>
#include "Reading.hpp"

template<typename T>
class Sensor {
 protected:
  String sModel = "";
  String sType = "";
  String sUnit = "";
  T rMin;
  T rMax;
  String sName = "";
  String sStatus = "init";
  uint32_t nErrors = 0;
  String sLastError = "";
  uint32_t nMeasurements = 0;
  Reading<T> *sReading = nullptr;

 public:

  Sensor(String name, String model, String type, String unit, T min, T max, Reading<T> *reading):
      sName(name),
      sModel(model),
      sType(type),
      sUnit(unit),
      rMin(min),
      rMax(max),
      sReading(reading)
  {}

  ~Sensor() {
    if (this->sReading != nullptr) {
      delete this->sReading;
    }
  }

  String model() const {
    return this->sModel;
  }

  String type() const {
    return this->sType;
  }

  String unit() const {
    return this->sUnit;
  }

  T rangeMin() const {
    return this->rMin;
  }

  T rangeMax() const {
    return this->rMax;
  }

  String name() const {
    return this->sName;
  }

  String status() const {
    return this->sStatus;
  }

  void setStatus(String status) {
    this->sStatus = status;
  }

  uint32_t errors() const {
    return this->nErrors;
  }

  String lastError() const {
    return this->sLastError;
  }

  void setError(String error) {
    this->sLastError = error;
    this->nErrors++;
    this->setStatus("error");
  }

  uint32_t measurements() const {
    return this->nMeasurements;
  }

  const Reading<T>* reading() const {
    return this->sReading;
  }

  String toJSON() const {
    String json = "{";
    json += "\"model\":\"" + this->model() + "\"";
    json += ",\"type\":\"" + this->type() + "\"";
    json += ",\"unit\":\"" + this->unit() + "\"";
    json += ",\"range\":{";
    json += "\"min\":" + String(this->rangeMin());
    json += ",\"max\":" + String(this->rangeMax());
    json += "}";
    json += ",\"name\":\"" + this->name() + "\"";
    json += ",\"status\":\"" + this->status() + "\"";
    json += ",\"errors\":" + String(this->errors());
    json += ",\"lastError\":\"" + this->lastError() + "\"";
    json += ",\"measurements\":" + String(this->measurements());
    json += ",\"reading\":" + this->reading()->toJSON();
    json += "}";
    return json;
  }

  void add(T s) {
    this->sReading->add(s);
    this->nMeasurements++;
  }
};

#endif
