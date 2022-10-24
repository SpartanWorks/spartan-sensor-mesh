#ifndef __HTUHUB_HPP__
#define __HTUHUB_HPP__

#include <Wire.h>
#include <SparkFunHTU21D.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class HTUHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  HTU21D sensor;
  Sensor humidity;
  Sensor temperature;

public:
  HTUHub(TwoWire *i2c, uint8_t addr);
  void begin();
  void update();
  void connect(Device *d) const;
};

#endif
