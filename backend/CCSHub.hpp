#ifndef __CCSHUB_HPP__
#define __CCSHUB_HPP__

#include "SoftwareSerial.h"
#include "Adafruit_CCS811.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class CCSHub: public SensorHub {
private:
  uint8_t address;
  uint8_t sda;
  uint8_t scl;
  Adafruit_CCS811 sensor;
  Sensor eco2;
  Sensor voc;
  Sensor temperature;

public:
  CCSHub(uint8_t da, uint8_t cl, uint8_t address);
  void begin();
  void update();
  void connect(Device *d) const;
};

#endif
