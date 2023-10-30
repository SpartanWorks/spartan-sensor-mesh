#ifndef __BUILT_IN_ADC_HPP_
#define __BUILT_IN_ADC_HPP_

#include <Arduino.h>
#include "ADCBackend.hpp"

// 1V ADC rail & 10-bit resolution.
#define MAX_RAW_READING_VALUE ((uint32_t)(1<<10)-1)
#define MAX_READING_VALUE 1.0

class BuiltInADC : public ADCBackend {
private:
  uint8_t numChannels;
  uint16_t *results;

public:
  BuiltInADC(uint8_t numChannels);
  ~BuiltInADC();

  bool read(uint8_t channel);
  uint16_t getReading(uint8_t channel);
  float getVoltage(uint8_t channel);
};

#endif
