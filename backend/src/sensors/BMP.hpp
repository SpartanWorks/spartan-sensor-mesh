#ifndef __BMP_HPP__
#define __BMP_HPP__

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "System.hpp"

class BMP: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_BMP280 sensor;
  uint16_t sampleInterval;
  Reading<float> *pressure;
  Reading<float> *temperature;
  Reading<float> *altitude;

  BMP(TwoWire *i2c, uint8_t addr, uint16_t interval);

 public:

  ~BMP();

  static BMP* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
