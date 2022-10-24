#ifndef __BMPHUB_HPP__
#define __BMPHUB_HPP__

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class BMPHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_BMP280 sensor;
  Sensor pressure;
  Sensor temperature;

public:
  BMPHub(TwoWire *i2c, uint8_t addr);
  void begin();
  void update();
  void connect(Device *d) const;
};

#endif
