#ifndef __ADC_OVER_UART_HPP__
#define __ADC_OVER_UART_HPP__

#include <Arduino.h>
#include "SoftwareSerial.h"
#include "ADCBackend.hpp"

// Protocol
#define ADC_OVER_UART_BAUDRATE 9600

#define HEADER 0x23
#define ACK 0x5

#define RESET 0x0
#define INIT 0x1
#define MEASURE 0x2
#define MEASURE_RUNNING 0x3
#define SET_SAMPLING_TIME 0x10
#define SET_DELTA_TIME 0x11
#define SET_SAMPLING_INTERVAL 0x12

#define MAX_CHANNELS 5

// 3V3 ADC rail & 16-bit resolution.
#define MAX_RAW_READING_VALUE ((uint32_t)(1<<16)-1)
#define MAX_READING_VALUE 3.3

class ADCOverUART : public ADCBackend {
private:
  SoftwareSerial *serial;
  uint8_t numChannels;
  uint16_t *results;

  bool send(uint8_t command);
  bool send(uint8_t command, uint8_t channel);
  bool send(uint8_t command, uint8_t channel, uint16_t value);
  bool readData(uint8_t channel);
  bool await(uint16_t time, uint8_t numBytes);
  bool ack();
  void flush();

public:
  ADCOverUART(uint8_t rx, uint8_t tx, uint8_t numChannels = MAX_CHANNELS);
  ~ADCOverUART();

  bool begin();
  bool reset();

  bool setSamplingInterval(uint8_t channel, uint16_t interval);
  bool setSamplingTime(uint8_t channel, uint16_t time);
  bool setDeltaTime(uint8_t channel, uint16_t time);

  bool read(uint8_t channel);
  uint16_t getReading(uint8_t channel);
  float getVoltage(uint8_t channel);
};

#endif
