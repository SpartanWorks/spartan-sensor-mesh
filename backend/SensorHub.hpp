#ifndef __SENSORHUB_HPP__
#define __SENSORHUB_HPP__

#include <Arduino.h>
#include "Sensor.hpp"

class Device;

class SensorHub {
public:
  virtual ~SensorHub() {}
  virtual void begin() = 0;
  virtual void update() = 0;
  virtual void connect(Device *d) const = 0;
  virtual void reset() {}
};

#endif
