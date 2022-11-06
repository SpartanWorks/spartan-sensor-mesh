#ifndef __CCS_HPP__
#define __CCS_HPP__

#include <Wire.h>
#include "SparkFunCCS811.h"
#include "System.hpp"

#define CCS_INIT_TIME 200
#define CCS_WARMUP_TIMEOUT 1200000 // 20 minutes
#define CCS_SAMPLE_INTERVAL 2000 // 2 seconds
#define SAMPLE_BACKLOG 30

class CCS: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  CCS811 sensor;
  Reading<float> *eco2;
  Reading<float> *voc;

  CCS(TwoWire *i2c, uint8_t address);
  void initSensor();

public:
  ~CCS();

  static CCS* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
  void reset();

  void setCompensationParameters(float temperature, float humidity = 50.0);
};

#endif
