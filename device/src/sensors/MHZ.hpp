#ifndef __MHZ_HPP__
#define __MHZ_HPP__

#include "SoftwareSerial.h"
#include "MHZ19.h"
#include "System.hpp"

#define MHZ_BAUDRATE 9600

class MHZ: public Sensor {
private:
  SoftwareSerial *serial;
  MHZ19 sensor;
  uint16_t sampleInterval;
  uint16_t warmupTime;
  Reading<float> *co2;
  Reading<float> *temperature;

  MHZ(uint8_t rx, uint8_t tx, uint16_t interval, uint16_t warmup);

  void initSensor();

public:
  ~MHZ();

  static MHZ* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
  void reset();
};

#endif
