#ifndef __METRIC_H__
#define __METRIC_H__

#include <Arduino.h>

template<typename T>
class Metric {
private:
  uint32_t countS = 0;
  T minS = (T) 0;
  T maxS = (T) 0;
  T avg = (T) 0;
  T var = (T) 0;
  T last = (T) 0;

public:
  Metric(T spanMin, T spanMax) {
    minS = spanMax;
    maxS = spanMin;
  }

  void add(T s) {
    T delta = s - avg;
    countS++;
    avg += delta / countS;
    var += delta * (s - avg);
    minS = std::min(s, minS);
    maxS = std::max(s, maxS);
    last = s;
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

  T value() {
    return last;
  }
};

template<typename T, uint16_t smoothingSize>
class SmoothMetric: public Metric<T> {
private:

  T movingAvg = (T) 0;
  T samples[smoothingSize];
  uint16_t index = 0;

public:
  SmoothMetric(T spanMin, T spanMax): Metric<T>(spanMin, spanMax) {
    for(uint16_t i = 0; i < smoothingSize; ++i) {
      samples[i] = (T) 0;
    }
  }

  void add(T s) {
    Metric<T>::add(s);
    movingAvg += s/smoothingSize - samples[index]/smoothingSize;
    samples[index] = s;
    index = (index + 1) % smoothingSize;
  }

  T value() {
    return movingAvg;
  }
};

#endif
