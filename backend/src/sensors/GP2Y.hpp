#ifndef __GP2Y_HPP__
#define __GP2Y_HPP__

#include <Arduino.h>
#include "SoftwareSerial.h"
#include "System.hpp"

// Protocol
#define GP2Y_BAUDRATE 9600

#define HEADER 0x23
#define ACK 0x5

#define RESET 0x0
#define INIT 0x1
#define MEASURE 0x2
#define MEASURE_RUNNING 0x3
#define SET_SAMPLING_TIME 0x10
#define SET_DELTA_TIME 0x11
#define SET_SAMPLING_INTERVAL 0x12

#define NUM_CHANNELS 1
#define SAMPLE_TIME 280
#define DELTA_TIME 40
#define SAMPLE_INTERVAL 10000

#define CHANNEL 0
#define GP2Y_SAMPLE_INTERVAL 1000
#define SAMPLE_BACKLOG 30

// 3.3 ADC rail & 16-bit resolution.
#define MAX_RAW_READING_VALUE ((uint32_t)(1<<16))
#define MAX_READING_VALUE 3.3

class GP2Y: public Sensor {
private:
  SoftwareSerial *serial;
  Reading<float> *pm;
  Reading<float> *raw;

  uint16_t readValue = 0;
  float offset = -0.293; // These two here calibrated against SSD011.
  float factor = 0.701;
  float rawScale = MAX_READING_VALUE/MAX_RAW_READING_VALUE;
  float baseline = 0.0;

  GP2Y(uint8_t rx, uint8_t tx);

  float getPM();

  bool init();
  bool read();

  bool send(uint8_t command);
  bool send(uint8_t command, uint8_t channel);
  bool send(uint8_t command, uint8_t channel, uint16_t value);
  bool readData();
  bool await(uint16_t time, uint8_t numBytes);
  bool ack();
  void flush();

public:
  ~GP2Y();

  static GP2Y* create(JSONVar &config);

  void begin(System &system);
  void update();
  void connect(Device *d) const;
};

#endif
