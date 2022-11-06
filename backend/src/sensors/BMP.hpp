#ifndef __BMP_HPP__
#define __BMP_HPP__

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "System.hpp"

#define BMP_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

class BMP: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_BMP280 sensor;
  Reading<float> *pressure;
  Reading<float> *temperature;
  Reading<float> *altitude;

  BMP(TwoWire *i2c, uint8_t addr);

 public:

  ~BMP();

  static BMP* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
