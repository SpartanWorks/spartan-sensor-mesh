#ifndef __SSN_HPP__
#define __SSN_HPP__

#include <LittleFS.h>
#include "System.hpp"

class SSN: public Sensor {
private:
  uint16_t sampleInterval;
  Reading<float> *ramFree;
  Reading<float> *fsFree;
  Reading<float> *uptime;

  SSN(uint16_t);

 public:

  ~SSN();

  static SSN* create(JSONVar &config);

  static uint32_t usedFSSize();
  static uint32_t totalFSSize();
  static uint32_t freeFSSize();
  static uint32_t totalHeapSize();
  static uint32_t freeHeapSize();

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
