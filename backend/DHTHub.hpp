#ifndef __DHTHUB_HPP__
#define __DHTHUB_HPP__

#include <DHT.h>
#include "System.hpp"

#define DHT_SAMPLE_INTERVAL 5000
#define SAMPLE_BACKLOG 30

class DHTHub: public SensorHub {
private:
  DHT sensor;
  Sensor<float> humidity;
  Sensor<float> temperature;

public:
  DHTHub(uint8_t pin, uint8_t model);
  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
