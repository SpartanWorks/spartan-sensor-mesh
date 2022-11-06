#ifndef __READING_HPP__
#define __READING_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Value.hpp"

template<typename T>
class Reading {
 protected:
  String sModel;
  String sType;
  String sName;
  String sStatus;
  uint32_t nErrors;
  String sLastError;
  uint32_t nMeasurements;
  Value<T> *sValue;

 public:

  Reading(String name, String model, String type, Value<T> *value):
      sModel(model),
      sType(type),
      sName(name),
      sStatus("init"),
      nErrors(0),
      sLastError(""),
      nMeasurements(0),
      sValue(value)
  {}

  virtual ~Reading() {
    if(sValue != nullptr) {
      delete sValue;
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

  const Value<T>* value() const {
    return this->sValue;
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

    JSONVar value = this->value()->toJSONVar();
    json["value"] = value;

    return json;
  }

  String toJSON() const {
    return JSON.stringify(this->toJSONVar());
  }

  void add(T s) {
    if (!isnan(s) && s <= this->sValue->rangeMax() && s >= this->sValue->rangeMin()) {
      this->sValue->add(s);
      this->nMeasurements++;
      this->setStatus("ok");
    } else {
      this->setError(String("Invalid sensor reading value: ") + s);
    }
  }
};

#endif
