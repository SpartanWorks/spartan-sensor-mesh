#ifndef __SENSORHUB_HPP__
#define __SENSORHUB_HPP__

#include <Arduino.h>
#include "Reading.hpp"

class Device;
class System;

class SensorHub {
public:
  virtual ~SensorHub() {}
  virtual void begin(System &system) = 0;
  virtual void update() = 0;
  virtual void connect(Device *d) const = 0;
  virtual void reset() {}

  virtual void setCompensationParameters(float temperature, float humidity = 50.0) {}
  virtual void compensate(SensorHub *other) {}
};

#endif
