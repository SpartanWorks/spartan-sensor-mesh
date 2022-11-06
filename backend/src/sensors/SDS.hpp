#ifndef __SDS_HPP__
#define __SDS_HPP__

#include <Arduino.h>
#include "SdsDustSensor.h"
#include "System.hpp"

#define SDS_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

class PatchedSdsSensor : public SdsDustSensor {
  HardwareSerial &serial;

  public:
  PatchedSdsSensor(HardwareSerial &hardwareSerial);
  void writeImmediate(const Command &command);
  void pollPm();
};

class SDS: public Sensor {
private:
  HardwareSerial &serial;
  PatchedSdsSensor sensor;
  Reading<float> pm25;
  Reading<float> pm10;

public:
  SDS(HardwareSerial &serial);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
