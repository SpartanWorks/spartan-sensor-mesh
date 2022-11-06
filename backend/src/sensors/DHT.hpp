#ifndef __DHT_HPP__
#define __DHT_HPP__

#include <DHT.h>

#include "System.hpp"

#define DHT_SAMPLE_INTERVAL 5000
#define SAMPLE_BACKLOG 30

namespace ssn {

class DHT: public Sensor {
 private:
  ::DHT sensor;
  Reading<float> *humidity;
  Reading<float> *temperature;

  DHT(uint8_t pin, uint8_t model);

 public:
  ~DHT();

  static DHT* create(JSONVar &config);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

}

#endif
