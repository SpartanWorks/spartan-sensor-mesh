#ifndef __HTU_HPP__
#define __HTU_HPP__

#include <Wire.h>
#include <SparkFunHTU21D.h>
#include "System.hpp"

#define HTU_SAMPLE_INTERVAL 2000
#define SAMPLE_BACKLOG 30

class HTU: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  HTU21D sensor;
  Reading<float> humidity;
  Reading<float> temperature;
  List<Sensor*> *toCompensate;

public:
  HTU(TwoWire *i2c, uint8_t addr);
  ~HTU();
  void begin(System &system);
  void update();
  void connect(Device *d) const;

  void compensate(Sensor *other);
};

#endif
