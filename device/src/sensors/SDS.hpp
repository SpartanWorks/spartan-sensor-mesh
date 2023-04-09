#ifndef __SDS_HPP__
#define __SDS_HPP__

#include <Arduino.h>
#include "SdsDustSensor.h"
#include "System.hpp"

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
  uint16_t sampleInterval;
  Reading<float> *pm25;
  Reading<float> *pm10;

  SDS(HardwareSerial &serial, uint16_t interval);
public:
  ~SDS();

  static SDS* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
