#ifndef __MHZHUB_HPP__
#define __MHZHUB_HPP__

#include "SoftwareSerial.h"
#include "MHZ19.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30
#define MHZ_BAUDRATE 9600

class MHZHub: public SensorHub {
private:
  SoftwareSerial *serial;
  MHZ19 sensor;
  Sensor<float> co2;
  Sensor<float> temperature;

  void initSensor();

public:
  MHZHub(uint8_t rx, uint8_t tx);
  void begin();
  void update();
  void connect(Device *d) const;
  void reset();
};

#endif
