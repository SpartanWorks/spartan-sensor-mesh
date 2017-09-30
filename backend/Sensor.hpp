#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <Arduino.h>

class Sensor {
protected:
  uint32_t nErrors = 0;
  uint32_t nMeasurements = 0;
  String sStatus = "";

public:
  virtual ~Sensor() {}
  virtual void begin() = 0;
  virtual void update() = 0;
  virtual String type() const = 0;
  virtual String status() const {
    return sStatus;
  }
  virtual uint32_t errors() const {
    return nErrors;
  }
  virtual uint32_t measurements() const {
    return nMeasurements;
  }
  virtual String toJSON() const = 0;
};

#endif
