#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <stdarg.h>

#define DEFAULT_BAUDRATE 115200

enum LogLevel {
  DEBUG = 3,
  INFO = 2,
  WARNING = 1,
  ERROR = 0
};

class Log {
 private:
  LogLevel currLevel;

 public:
  Log(LogLevel level): currLevel(level) {}

  void setLevel(LogLevel level) {
    currLevel = level;
  }

  void begin() {
    Serial.begin(115200);
  }

  void print(String message) {
    Serial.print(message);
  }

  void println(String message) {
    Serial.println(message);
  }

#define DEFINE_LOGGER(level, name)   \
  void name(String message) {        \
    if(currLevel >= (level)) {       \
      print("[" #level "] ");        \
      println(message);              \
    }                                \
  }                                  \
  void name(const char *input...) {  \
    va_list args;                    \
    va_start(args, input);           \
    char buf[255];                   \
    vsnprintf(buf, 255, input, args);\
    name(String(buf));               \
  }                                  \
  void name(JSONVar input) {         \
    name(JSON.stringify(input));     \
  }                                  \

  DEFINE_LOGGER(DEBUG, debug)
  DEFINE_LOGGER(INFO, info)
  DEFINE_LOGGER(WARNING, warn)
  DEFINE_LOGGER(ERROR, error)
};

#endif
