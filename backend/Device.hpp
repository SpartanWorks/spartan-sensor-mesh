#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <Arduino.h>

class Device {
private:
  String dName;
  String dGroup;

public:
  Device(String n, String g);
  virtual void begin();
  virtual void update();
  virtual String model() const;
  virtual String name() const;
  virtual String group() const;
  virtual String toJSON() const;
};

#endif
