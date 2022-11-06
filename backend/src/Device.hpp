#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Reading.hpp"
#include "SensorHub.hpp"
#include "List.hpp"

class Device {
private:
  String dModel;
  String dName;
  String dPass;
  String dGroup;
  List<const Reading<float>*> *list; // FIXME Could use Reading<any> here.

public:
  Device();
  Device(String m, String p);
  Device(String m, String n, String p);
  Device(String m, String g, String n, String p);
  ~Device();

  virtual void attach(const Reading<float> *s);
  virtual void attach(const SensorHub *s);
  virtual String model() const;
  virtual String name() const;
  virtual String password() const;
  virtual String group() const;
  virtual JSONVar toJSONVar() const;
  virtual String toJSON() const;
};

#endif
