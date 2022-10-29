#ifndef __HTUHUB_HPP__
#define __HTUHUB_HPP__

#include <Wire.h>
#include <SparkFunHTU21D.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"
#include "List.hpp"

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
  void begin();
  void update();
  void connect(Device *d) const;

  void compensate(SensorHub *other);
};

#endif
