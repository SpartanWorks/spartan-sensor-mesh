#ifndef __GP2Y_HPP__
#define __GP2Y_HPP__

#include <Arduino.h>
#include "ADCOverUART.hpp"
#include "System.hpp"

#define NUM_CHANNELS 1
#define CHANNEL 0

#define SAMPLE_TIME 280
#define DELTA_TIME 40
#define SAMPLE_INTERVAL 10000

#define GP2Y_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

class GP2Y: public Sensor {
private:
  ADCOverUART sensor;
  Reading<float> *pm;
  Reading<float> *raw;

  float offset = -0.293; // These two here calibrated against SSD011.
  float factor = 0.701;
  float baseline = 0.0;

  GP2Y(uint8_t rx, uint8_t tx);

  float getPM();
public:
  ~GP2Y();

  static GP2Y* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
