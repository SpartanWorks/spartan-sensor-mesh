#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <Arduino.h>
#include "Reading.hpp"

class Sensor {
protected:
  String sModel = "";
  String sType = "";
  String sName = "";
  String sStatus = "init";
  uint32_t nErrors = 0;
  uint32_t nMeasurements = 0;
  Reading<float> &sReading;

public:
  Sensor(String model, String type, String name, Reading<float> &reading);
  virtual String model() const;
  virtual String type() const;
  virtual String name() const;
  virtual String status() const;
  virtual uint32_t errors() const;
  virtual uint32_t measurements() const;
  virtual Reading<float> reading() const;
  virtual String toJSON() const;
};

#endif
