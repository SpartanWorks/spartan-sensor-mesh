#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Reading.hpp"

template<typename T>
class Sensor {
 protected:
  String sModel;
  String sType;
  String sName;
  String sStatus;
  uint32_t nErrors;
  String sLastError;
  uint32_t nMeasurements;
  Reading<T> *sReading;

 public:

  Sensor(String name, String model, String type, Reading<T> *reading):
      sModel(model),
      sType(type),
      sName(name),
      sStatus("init"),
      nErrors(0),
      sLastError(""),
      nMeasurements(0),
      sReading(reading)
  {}

  virtual ~Sensor() {
    if(sReading != nullptr) {
      delete sReading;
    }
  }

  String model() const {
    return this->sModel;
  }

  String type() const {
    return this->sType;
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

  JSONVar toJSONVar() const {
    JSONVar json;

    json["model"] = this->model();
    json["type"] = this->type();
    json["name"] = this->name();
    json["status"] = this->status();
    json["errors"] = (unsigned long) this->errors();
    json["lastError"] = this->lastError();
    json["measurements"] = (unsigned long) this->measurements();

    JSONVar reading = this->reading()->toJSONVar();
    json["reading"] = reading;

    return json;
  }

  String toJSON() const {
    return JSON.stringify(this->toJSONVar());
  }

  void add(T s) {
  if (!isnan(s)) {
    this->sReading->add(s);
    this->nMeasurements++;
    this->setStatus("ok");
  } else {
    this->setError("Invalid sensor reading value.");
  }
}

};

#endif
