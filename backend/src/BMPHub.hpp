#ifndef __BMPHUB_HPP__
#define __BMPHUB_HPP__

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "System.hpp"

#define BMP_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

class BMPHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_BMP280 sensor;
  Reading<float> pressure;
  Reading<float> temperature;
  Reading<float> altitude;

public:
  BMPHub(TwoWire *i2c, uint8_t addr);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
