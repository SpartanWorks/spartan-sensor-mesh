#ifndef __SGP_HPP__
#define __SGP_HPP__

#include <Wire.h>
#include "SGP30.h"
#include "System.hpp"

#define SGP_INIT_TIME 200
#define SGP_WARMUP_TIMEOUT 60000 // 1 minute
#define SGP_SAMPLE_INTERVAL 2000 // 2 seconds
#define SAMPLE_BACKLOG 30

class SGP: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  SGP30 sensor;
  Reading<float> voc;
  Reading<float> co2;
  Reading<float> h2;
  Reading<float> ethanol;

public:
  SGP(TwoWire *i2c, uint8_t address);
  void begin(System &system);
  void update();
  void connect(Device *d) const;

  void setCompensationParameters(float temperature, float humidity = 50.0);
};

#endif
