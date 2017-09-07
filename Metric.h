#ifndef __STATS_H__
#define __STATS_H__

#include <Arduino.h>

template<typename T, uint16_t size>
class Metric {
private:
  T samples[size];
  uint16_t index = 0;
  uint32_t countS = 0;
  T minS;
  T maxS;
  T avg;
  T var;

public:
  Metric(T spanMin, T spanMax) {
    minS = spanMax;
    maxS = spanMin;
    avg = (T) 0;
    var = (T) 0;
  }

  void add(T s) {
    T delta = s - avg;
    countS++;
    avg += delta / countS;
    var += delta * (s - avg);
    minS = std::min(s, minS);
    maxS = std::max(s, maxS);
    samples[index] = s;
    index = (index + 1) % size;
  }

  T minimum() {
    return minS;
  }

  T maximum() {
    return maxS;
  }

  T average() {
    return avg;
  }

  T variance() {
    return (countS > 1) ? (var / (countS - 1)) : ((T) 0);
  }

  T last() {
    uint16_t i = (index + size - 1) % size;
    return samples[i];
  }

  T value() {
    T avg;
    for(uint16_t i = 0; i < size; ++i) {
      avg += samples[i];
    }
    return avg / size;
  }

  uint16_t count() {
    return countS;
  }
};

#endif
