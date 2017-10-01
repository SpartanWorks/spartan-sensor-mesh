#ifndef __DHTSENSOR_HPP__
#define __DHTSENSOR_HPP__

#include <DHT.h>
#include "Sensor.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class DHTSensor: public Sensor {
private:
  WindowedReading<float, SAMPLE_BACKLOG> humidity;
  WindowedReading<float, SAMPLE_BACKLOG> temperature;
  DHT sensor;

public:
  DHTSensor(uint8_t pin, uint8_t model);
  void begin();
  void update();
  String toJSON() const;
};

#endif
