#ifndef __CCSHUB_HPP__
#define __CCSHUB_HPP__

#include <Wire.h>
#include "SparkFunCCS811.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define INIT_TIME 200
#define SAMPLE_BACKLOG 30

class CCSHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  CCS811 sensor;
  Sensor<float> eco2;
  Sensor<float> voc;

  void initSensor();

public:
  CCSHub(TwoWire *i2c, uint8_t address);
  void begin();
  void update();
  void connect(Device *d) const;
  void reset();

  void setCompensationParameters(float temperature, float humidity = 50.0);
};

#endif
