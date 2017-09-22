#ifndef __DHTSENSOR_HPP__
#define __DHTSENSOR_HPP__

#include <DHT.h>
#include "Sensor.hpp"
#include "Metric.hpp"

#define SAMPLE_BACKLOG 30

class DHTSensor: public Sensor {
private:
  boolean error = true;
  uint32_t nErrors = 0;
  uint32_t nMeasurements = 0;
  WindowedMetric<float, SAMPLE_BACKLOG> humidity;
  WindowedMetric<float, SAMPLE_BACKLOG> temperature;
  DHT sensor;

public:
  DHTSensor(uint16_t pin, uint16_t model);
  void begin();
  void update();
  String type() const;
  String status() const;
  uint32_t errors() const;
  uint32_t measurements() const;
  String toJSON() const;
};

#endif
