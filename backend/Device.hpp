#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <Arduino.h>

class Device {
private:
  String dName;
  String dPass;
  String dGroup;

public:
  Device(String p);
  Device(String n, String p);
  Device(String n, String p, String g);
  virtual void begin();
  virtual void update();
  virtual String model() const;
  virtual String name() const;
  virtual String password() const;
  virtual String group() const;
  virtual String toJSON() const;
};

#endif
