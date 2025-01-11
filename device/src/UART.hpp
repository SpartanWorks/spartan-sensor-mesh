#ifndef __UART_HPP__
#define __UART_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>

class HardwareUART;
class SoftwareUART;

class UART {
 public:
  virtual void begin(uint32_t baudRate) = 0;
  virtual Stream *getStream() = 0;
  virtual ~UART() {}

  static UART* create(JSONVar &config);
};

class HardwareUART : public UART {
  HardwareSerial &serial;

 public:
  HardwareUART(HardwareSerial &serial) : serial(serial) {}

  void begin(uint32_t baudRate) {
    serial.begin(baudRate);
  }

  Stream* getStream() {
    return &serial;
  }
};

class SoftwareUART : public UART {
  SoftwareSerial* serial;

 public:
  SoftwareUART(uint8_t rx, uint8_t tx) : serial(nullptr) {
    serial = new SoftwareSerial(rx, tx);
  }

  ~SoftwareUART() {
    if (serial != nullptr) {
      delete serial;
    }
  }

  void begin(uint32_t baudRate) {
    serial->begin(baudRate);
  }

  Stream* getStream() {
    return serial;
  }

};

#endif
