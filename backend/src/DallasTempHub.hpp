#ifndef __DALLASTEMPHUB_HPP__
#define __DALLASTEMPHUB_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "System.hpp"

#define DALLAS_SAMPLE_INTERVAL 5000
#define SAMPLE_BACKLOG 30

struct Temp {
  uint8_t index;
  Reading<float> *reading;

  Temp(int i, Reading<float> *r): index(i), reading(r) {}
};

class DallasTempHub: public SensorHub {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  uint8_t nReadings = 0;
  List<Temp> *temperatures = nullptr;

public:
  DallasTempHub(uint8_t pin, uint8_t resolution);
  ~DallasTempHub();
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
