#ifndef __DHTHUB_HPP__
#define __DHTHUB_HPP__

#include <DHT.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class DHTHub: public SensorHub {
private:
  WindowedReading<float, SAMPLE_BACKLOG> humidity;
  WindowedReading<float, SAMPLE_BACKLOG> temperature;
  DHT sensor;

public:
  DHTHub(uint8_t pin, uint8_t model);
  void begin();
  void update();
  void connect(Device *d);
};

#endif
