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
  DHT sensor;
  Sensor humidity;
  Sensor temperature;

public:
  DHTHub(uint8_t pin, uint8_t model);
  void begin();
  void update();
  void connect(Device *d);
};

#endif
