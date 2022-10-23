#ifndef __SDSHUB_HPP__
#define __SDSHUB_HPP__

#include <Arduino.h>
#include "SdsDustSensor.h"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class SDSHub: public SensorHub {
private:
  HardwareSerial &serial;
  SdsDustSensor sensor;
  Sensor pm25;
  Sensor pm10;

public:
  SDSHub(HardwareSerial &serial);
  void begin();
  void update();
  void connect(Device *d) const;
};

#endif
