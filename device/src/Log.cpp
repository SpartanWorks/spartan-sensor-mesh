#include "Log.hpp"

int swallowSadness(char *input, va_list args) {
  return 0;
}

int printToSerial0(char *input, va_list args) {
  char buf[512];
  int result = vsnprintf(buf, 512, input, args);
  Serial.print(buf);
  return result;
}

int printToSerial1(char *input, va_list args) {
  char buf[512];
  int result = vsnprintf(buf, 512, input, args);
  Serial1.print(buf);
  return result;
}

vprintf_like_t thePrint = (vprintf_like_t) &swallowSadness;
