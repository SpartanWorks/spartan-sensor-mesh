#include "Sensor.hpp"

template<>
void Sensor<float>::add(float s) {
  if (!isnan(s)) {
    this->sReading->add(s);
    this->nMeasurements++;
    this->setStatus("ok");
  } else {
    this->setError("Invalid sensor reading value.");
  }
}
