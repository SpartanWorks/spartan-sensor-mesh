#ifndef __ZE25O3_HPP__
#define __ZE25O3_HPP__

#include <Arduino.h>
#include "System.hpp"
#include "UART.hpp"

#define BAUD_RATE 9600
#define RESP_LEN 9

class ZE25O3: public Sensor {
private:
  UART *serial;
  uint16_t sampleInterval;
  Reading<float> *ozone;

  ZE25O3(UART *serial, uint16_t interval);

public:
  ~ZE25O3();

  static ZE25O3* create(JSONVar &config);

  void initSensor();
  float readSensor();

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
