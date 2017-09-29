#ifndef __DALLAS_TEMP_SENSOR_HPP__
#define __DALLAS_TEMP_SENSOR_HPP__

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.hpp"
#include "Reading.hpp"

class DallasTempSensor: public Sensor {
private:
  Reading<float> temperature;
  OneWire oneWire;
  DallasTemperature sensor;

public:
  DallasTempSensor(uint8_t pin, uint8_t resolution);
  void begin();
  void update();
  String type() const;
  String toJSON() const;
};

#endif
