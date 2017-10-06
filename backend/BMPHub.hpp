#ifndef __BMPHUB_HPP__
#define __BMPHUB_HPP__

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Device.hpp"
#include "Reading.hpp"

#define SAMPLE_BACKLOG 30

class BMPHub: public SensorHub {
private:
  uint8_t address;
  uint8_t sda;
  uint8_t scl;
  Adafruit_BMP280 sensor;
  WindowedReading<float, SAMPLE_BACKLOG> pressure;
  WindowedReading<float, SAMPLE_BACKLOG> temperature;

public:
  BMPHub(uint8_t sda, uint8_t scl, uint8_t addr);
  void begin();
  void update();
  void connect(Device *d);
};

#endif
