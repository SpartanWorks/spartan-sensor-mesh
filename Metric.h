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
  T movingAvg;
  T var;

public:
  Metric(T spanMin, T spanMax) {
    minS = spanMax;
    maxS = spanMin;
    movingAvg = (T) 0;
    avg = (T) 0;
    var = (T) 0;

    for(uint16_t i = 0; i < size; ++i) {
      samples[i] = (T) 0;
    }
  }

  void add(T s) {
    T delta = s - avg;
    countS++;
    avg += delta / countS;
    var += delta * (s - avg);
    minS = std::min(s, minS);
    maxS = std::max(s, maxS);
    movingAvg += s/size - samples[index]/size;
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
    return movingAvg;
  }

  uint16_t count() {
    return countS;
  }
};

#endif
