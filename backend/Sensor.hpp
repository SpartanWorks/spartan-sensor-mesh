#ifndef __SENSOR_HPP__
#define __SENSOR_HPP__

#include <Arduino.h>

class Sensor {
public:
  virtual void begin() = 0;
  virtual void update() = 0;
  virtual String type() const = 0;
  virtual String status() const = 0;
  virtual uint32_t errors() const = 0;
  virtual uint32_t measurements() const = 0;
  virtual String toJSON() const = 0;
};

#endif
