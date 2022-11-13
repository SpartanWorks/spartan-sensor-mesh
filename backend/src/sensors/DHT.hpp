#ifndef __DHT_HPP__
#define __DHT_HPP__

#include <DHT.h>

#include "System.hpp"

namespace ssn {

class DHT: public Sensor {
 private:
  ::DHT sensor;
  uint16_t sampleInterval;
  Reading<float> *humidity;
  Reading<float> *temperature;

  DHT(uint8_t pin, uint8_t model, uint16_t interval);

 public:
  ~DHT();

  static DHT* create(JSONVar &config);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

}

#endif
