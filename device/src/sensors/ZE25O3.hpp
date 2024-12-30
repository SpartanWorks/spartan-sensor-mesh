#ifndef __ZE25O3_HPP__
#define __ZE25O3_HPP__

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ZE27.h>
#include "System.hpp"

class ZE25O3: public Sensor {
private:
  SoftwareSerial *serial;
  ZE27 *sensor;
  uint16_t sampleInterval;
  Reading<float> *ozone;

  ZE25O3(uint8_t rx, uint8_t tx, uint16_t interval);
  ZE25O3(HardwareSerial &serial, uint16_t interval);

public:
  ~ZE25O3();

  static ZE25O3* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
