#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "List.hpp"

class Device {
private:
  String dName;
  String dPass;
  String dGroup;
  List<const Sensor<float>*> *list; // FIXME Could use Sensor<any> here.

public:
  Device(String p);
  Device(String n, String p);
  Device(String n, String p, String g);
  ~Device();

  virtual void attach(const Sensor<float> *s);
  virtual void attach(const SensorHub *s);
  virtual String model() const;
  virtual String name() const;
  virtual String password() const;
  virtual String group() const;
  virtual JSONVar toJSONVar() const;
  virtual String toJSON() const;
};

#endif
