#ifndef __CCS_HPP__
#define __CCS_HPP__

#include <Wire.h>
#include "SparkFunCCS811.h"
#include "System.hpp"

#define CCS_INIT_TIME 200

class CCS: public Sensor {
private:
  TwoWire *i2c;
  uint8_t address;
  CCS811 sensor;
  uint16_t sampleInterval;
  uint16_t warmupTime;
  Reading<float> *eco2;
  Reading<float> *voc;

  CCS(TwoWire *i2c, uint8_t address, uint16_t ws, uint16_t wt);
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
