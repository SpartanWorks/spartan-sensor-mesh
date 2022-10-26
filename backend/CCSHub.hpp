#ifndef __CCSHUB_HPP__
#define __CCSHUB_HPP__

#include <Wire.h>
#include "Adafruit_CCS811.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class CCSHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_CCS811 sensor;
  Sensor eco2;
  Sensor voc;

  void initSensor();

public:
  CCSHub(TwoWire *i2c, uint8_t address);
  void begin();
  void update();
  void connect(Device *d) const;
  void reset();
};

#endif
