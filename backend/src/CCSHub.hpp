#ifndef __CCSHUB_HPP__
#define __CCSHUB_HPP__

#include <Wire.h>
#include "SparkFunCCS811.h"
#include "System.hpp"

#define CCS_INIT_TIME 200
#define CCS_WARMUP_TIMEOUT 1200000 // 20 minutes
#define CCS_SAMPLE_INTERVAL 2000 // 2 seconds
#define SAMPLE_BACKLOG 30

class CCSHub: public SensorHub {
private:
  TwoWire *i2c;
  uint8_t address;
  CCS811 sensor;
  Sensor<float> eco2;
  Sensor<float> voc;

  void initSensor();

public:
  CCSHub(TwoWire *i2c, uint8_t address);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
  void reset();

  void setCompensationParameters(float temperature, float humidity = 50.0);
};

#endif
