#ifndef __MHZ_HPP__
#define __MHZ_HPP__

#include "SoftwareSerial.h"
#include "MHZ19.h"
#include "System.hpp"

#define MHZ_BAUDRATE 9600

#define MHZ_WARMUP_TIMEOUT 1200000 // 20 minutes
#define MHZ_SAMPLE_INTERVAL 2000
#define SAMPLE_BACKLOG 30

class MHZ: public Sensor {
private:
  SoftwareSerial *serial;
  MHZ19 sensor;
  Reading<float> *co2;
  Reading<float> *temperature;

  MHZ(uint8_t rx, uint8_t tx);

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
