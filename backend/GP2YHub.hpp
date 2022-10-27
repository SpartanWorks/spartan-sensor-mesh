#ifndef __GP2YHUB_HPP__
#define __GP2YHUB_HPP__

#include <Arduino.h>
#include "SoftwareSerial.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define HEADER 0x23
#define ACK 0x5
#define RESET 0x0
#define MEASURE 0x1
#define MEASURE_RAW 0x2

#define SAMPLING 10

#define SAMPLE_BACKLOG 30
#define GP2Y_BAUDRATE 9600

class GP2YHub: public SensorHub {
private:
  SoftwareSerial *serial;
  Sensor pm;

  uint16_t readValue = 0;
  float offset = -0.05;
  float factor = 0.172;
  float rawScale = 3.3/1024; // 3.3 ADC rail & 10-bit resolution.
  bool read();
  float getPM();

public:
  GP2YHub(uint8_t rx, uint8_t tx);
  void begin();
  void update();
  void connect(Device *d) const;
};

#endif
