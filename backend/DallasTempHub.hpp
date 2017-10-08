#ifndef __DALLASTEMPHUB_HPP__
#define __DALLASTEMPHUB_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"
#include "List.hpp"

#define SAMPLE_BACKLOG 30

struct Temp {
  uint8_t index = 0;
  Sensor *sensor = nullptr;

  Temp(int i, Sensor *s): sensor(s), index(i) {}
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
  void begin();
  void update();
  void connect(Device *d);
};

#endif
