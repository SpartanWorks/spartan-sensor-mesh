#ifndef __DALLASTEMPHUB_HPP__
#define __DALLASTEMPHUB_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class DallasTempHub: public SensorHub {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  uint8_t nSensors = 0;
  WindowedReading<float, SAMPLE_BACKLOG> *temperatures;

public:
  DallasTempHub(uint8_t pin, uint8_t resolution);
  ~DallasTempHub();
  void begin();
  void update();
  void connect(Device *d);
};

#endif
