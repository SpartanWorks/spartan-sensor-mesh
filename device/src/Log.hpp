#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <stdarg.h>

#ifdef ESP32
#include <esp_log.h>
#else
typedef int (*vprintf_like_t)(const char *, va_list);
#endif

extern vprintf_like_t thePrint;

int printToSerial0(char *input, va_list args);
int printToSerial1(char *, va_list);

#define DEFAULT_BAUDRATE 115200

enum LogLevel {
  NONE = 0,
  ERROR,
  WARNING,
  INFO,
  DEBUG,
  VERBOSE
};

class Log {
 private:
  LogLevel currLevel = NONE;

 public:
  void setLevel(LogLevel level) {
    currLevel = level;
  }

  void begin(JSONVar &config) {
    LogLevel level = DEBUG;

    if(config.hasOwnProperty("level")) {
      String l = (const char *) config["level"];
      if(l == "verbose") {
        level = VERBOSE;
      } else if (l == "debug") {
        level = DEBUG;
      } else if (l == "info") {
        level = INFO;
      } else if (l == "warning") {
        level = WARNING;
      } else if (l == "error") {
        level = ERROR;
      } else {
        level = NONE;
      }
    }

    if(config.hasOwnProperty("console")) {
      JSONVar c = config["console"];

      uint32_t baudrate = DEFAULT_BAUDRATE;
      if(c.hasOwnProperty("baudrate")) {
        baudrate = (unsigned long) c["baudrate"];
      }

      if(String((const char*) c["type"]) == "hardware-uart" && (int) c["number"] == 1) {
        Serial1.begin(baudrate);
        thePrint = (vprintf_like_t) &printToSerial1;
      } else {
        Serial.begin(baudrate);
        thePrint = (vprintf_like_t) &printToSerial0;
      }
    }

    setLevel(level);

#ifdef ESP32
    esp_log_level_set("*", (esp_log_level_t) level);
    esp_log_set_vprintf(thePrint);
#endif

#ifdef ESP8266
    // TODO
    // Seems to only be configurable during compile time.
    // https://github.com/esp8266/Arduino/blob/master/doc/Troubleshooting/debugging.rst
    // An alternative would be to switch to the esp8266-rtos-sdk.
#endif
  }

  void print(String message) {
    va_list args;
    thePrint(message.c_str(), args);
  }

  void println(String message) {
    print(message + "\r\n");
  }

#define DEFINE_LOGGER(level, name)   \
  void name(String message) {        \
    if(currLevel >= (level)) {       \
      print("[" #level "] ");        \
      println(message);              \
    }                                \
  }                                  \
  void name(const char *input...) {  \
    if(currLevel >= (level)) {       \
      print("[" #level "] ");        \
      va_list args;                  \
      va_start(args, input);         \
      thePrint(input, args);         \
      va_end(args);                  \
      print("\n");                   \
    }                                \
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
