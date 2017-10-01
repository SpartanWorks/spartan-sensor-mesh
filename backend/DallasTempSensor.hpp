#ifndef __DALLAS_TEMP_SENSOR_HPP__
#define __DALLAS_TEMP_SENSOR_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class DallasTempSensor: public Sensor {
private:
  OneWire oneWire;
  DallasTemperature sensors;
  uint8_t nSensors = 0;
  WindowedReading<float, SAMPLE_BACKLOG> *temperatures;

public:
  DallasTempSensor(uint8_t pin, uint8_t resolution);
  ~DallasTempSensor();
  void begin();
  void update();
  String toJSON() const;
};

#endif
