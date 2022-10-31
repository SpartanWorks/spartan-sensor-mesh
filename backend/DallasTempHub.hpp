#ifndef __DALLASTEMPHUB_HPP__
#define __DALLASTEMPHUB_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "System.hpp"

#define DALLAS_SAMPLE_INTERVAL 5000
#define SAMPLE_BACKLOG 30

struct Temp {
  uint8_t index = 0;
  Sensor<float> *sensor = nullptr;

  Temp(int i, Sensor<float> *s): sensor(s), index(i) {}
};

class DallasTempHub: public SensorHub {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  uint8_t nSensors = 0;
  List<Temp> *temperatures = nullptr;

public:
  DallasTempHub(uint8_t pin, uint8_t resolution);
  ~DallasTempHub();
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
