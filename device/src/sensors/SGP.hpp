#ifndef __SGP_HPP__
#define __SGP_HPP__

#include <Wire.h>
#include "SGP30.h"
#include "System.hpp"

#define SGP_INIT_TIME 200

class SGP: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  SGP30 sensor;
  uint16_t sampleInterval;
  uint16_t warmupTime;
  Reading<float> *voc;
  Reading<float> *co2;
  Reading<float> *h2;
  Reading<float> *ethanol;

  SGP(TwoWire *i2c, uint8_t address, uint16_t interval, uint16_t warmup);

public:
  ~SGP();

  static SGP* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;

  void setCompensationParameters(float temperature, float humidity = 50.0);
};

#endif
