#ifndef __ADC_BACKEND_HPP__
#define __ADC_BACKEND_HPP__

#include <Arduino.h>

class ADCBackend {
public:
  virtual ~ADCBackend() {}

  virtual bool begin() {
    return true;
  }

  virtual bool reset() {
    return true;
  }

  virtual bool setSamplingInterval(uint8_t channel, uint16_t interval) {
    return true;
  }
  virtual bool setSamplingTime(uint8_t channel, uint16_t time) {
    return true;
  }
  virtual bool setDeltaTime(uint8_t channel, uint16_t time) {
    return true;
  }

  virtual bool read(uint8_t channel) = 0;
  virtual uint16_t getReading(uint8_t channel) = 0;
  virtual float getVoltage(uint8_t channel) = 0;
};

#endif
