#ifndef __ADC_HPP__
#define __ADC_HPP__

#include <Arduino.h>
#include "ADCOverUART.hpp"
#include "System.hpp"

#define ADC_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

struct ADCChannel {
  uint8_t index;

  uint16_t samplingInterval;
  uint16_t samplingTime;
  uint16_t deltaTime;

  float min;
  float offset;
  float factor;
  float baseline;

  Reading<float> *scaled;
  Reading<float> *raw;

  ADCChannel(JSONVar &config, Reading<float> *s, Reading<float> *r);

  void add(uint16_t raw, float volts);
  void setError(String error);
};

class ADC: public Sensor {
private:
  ADCOverUART sensor;
  List<ADCChannel> *channels = nullptr;

  ADC(uint8_t rx, uint8_t tx, uint8_t numChannels, List<ADCChannel> *channels);

public:
  ~ADC();

  static ADC* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
