#ifndef __HTUHUB_HPP__
#define __HTUHUB_HPP__

#include <Wire.h>
#include <SparkFunHTU21D.h>
#include "System.hpp"

#define HTU_SAMPLE_INTERVAL 2000
#define SAMPLE_BACKLOG 30

class HTUHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  HTU21D sensor;
  Sensor<float> humidity;
  Sensor<float> temperature;
  List<SensorHub*> *toCompensate;

public:
  HTUHub(TwoWire *i2c, uint8_t addr);
  ~HTUHub();
  void begin(System &system);
  void update();
  void connect(Device *d) const;

  void compensate(SensorHub *other);
};

#endif
