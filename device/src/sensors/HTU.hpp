#ifndef __HTU_HPP__
#define __HTU_HPP__

#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include "System.hpp"

class HTU: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  Adafruit_HTU21DF sensor;
  uint16_t sampleInterval;
  Reading<float> *humidity;
  Reading<float> *temperature;
  List<Sensor*> *toCompensate;

  HTU(TwoWire *i2c, uint8_t addr, uint16_t interval);

public:

  static HTU* create(JSONVar &config);

  ~HTU();
  void begin(System &system);
  void update();
  void connect(Device *d) const;

  void compensate(Sensor *other);
};

#endif
