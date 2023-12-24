#ifndef __BUILT_IN_ADC_HPP_
#define __BUILT_IN_ADC_HPP_

#include <Arduino.h>
#include "ADCBackend.hpp"

#ifdef ESP32
// 3V3 ADC rail & 12-bit resolution.
#define BUILT_IN_ADC_MAX_RAW_READING_VALUE ((uint32_t)(1<<12)-1)
#define BUILT_IN_ADC_MAX_READING_VALUE 3.3
#endif

#ifdef ESP8266
// 1V ADC rail & 10-bit resolution.
#define BUILT_IN_ADC_MAX_RAW_READING_VALUE ((uint32_t)(1<<10)-1)
#define BUILT_IN_ADC_MAX_READING_VALUE 1.0
#endif

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
