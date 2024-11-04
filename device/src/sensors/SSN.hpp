#ifndef __SSN_HPP__
#define __SSN_HPP__

#include <LittleFS.h>
#include "System.hpp"

class SSN: public Sensor {
private:
  uint16_t sampleInterval;
  Reading<float> *ramFree;
  Reading<float> *fsFree;

  SSN(uint16_t);

 public:

  ~SSN();

  static SSN* create(JSONVar &config);

  static uint32_t fsUsedBytes();
  static uint32_t fsTotalBytes();
  static uint32_t fsFreeBytes();

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
