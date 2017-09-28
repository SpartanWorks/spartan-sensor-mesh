#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <functional>
#include <Arduino.h>
#include "Sensor.hpp"

template<typename T>
class List {
public:
  T item;
  List<T> *next = nullptr;

  List(T i, List<T> *n): item(i), next(n) {
  }
};

template<typename T>
void foreach(const List<T> *i, std::function<void(T)> f) {
  while(i != nullptr) {
    f(i->item);
    i = i->next;
  }
}

class Device {
private:
  String dName;
  String dPass;
  String dGroup;
  List<Sensor*> *list;

public:
  Device(String p);
  Device(String n, String p);
  Device(String n, String p, String g);

  virtual void attach(Sensor *s);
  virtual void begin();
  virtual void update();
  virtual String model() const;
  virtual String name() const;
  virtual String password() const;
  virtual String group() const;
  virtual String toJSON() const;
};

#endif
