#include "BuiltInADC.hpp"

BuiltInADC::BuiltInADC(uint8_t numChannels):
    numChannels(numChannels)
{
  this->results = new uint16_t[numChannels];
}

BuiltInADC::~BuiltInADC() {
  if (this->results != nullptr) delete this->results;
}

bool BuiltInADC::read(uint8_t channel) {
  this->results[channel] = analogRead(A0 + channel);
  return true;
}

uint16_t BuiltInADC::getReading(uint8_t channel) {
  return this->results[channel];
}

float BuiltInADC::getVoltage(uint8_t channel) {
  return this->results[channel] * BUILT_IN_ADC_MAX_READING_VALUE / BUILT_IN_ADC_MAX_RAW_READING_VALUE;
}
