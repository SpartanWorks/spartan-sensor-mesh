#ifndef __DALLASTEMP_HPP__
#define __DALLASTEMP_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "System.hpp"

struct Temp {
  uint8_t index;
  Reading<float> *reading;

  Temp(int i, Reading<float> *r): index(i), reading(r) {}
};

class DallasTemp: public Sensor {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  uint16_t sampleInterval;
  uint8_t nReadings = 0;
  List<Temp> *temperatures = nullptr;

  DallasTemp(uint8_t pin, uint8_t resolution, uint16_t sampleInterval);

public:
  ~DallasTemp();

  static DallasTemp* create(JSONVar &config);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
