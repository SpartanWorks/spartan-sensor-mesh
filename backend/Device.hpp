#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <Arduino.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "List.hpp"

class Device {
private:
  String dName;
  String dPass;
  String dGroup;
  List<const Sensor*> *list;

public:
  Device(String p);
  Device(String n, String p);
  Device(String n, String p, String g);
  ~Device();

  virtual void attach(const Sensor *s);
  virtual void attach(const SensorHub *s);
  virtual String model() const;
  virtual String name() const;
  virtual String password() const;
  virtual String group() const;
  virtual String toJSON() const;
};

#endif
