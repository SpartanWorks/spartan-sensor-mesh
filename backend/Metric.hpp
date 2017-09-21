#ifndef __METRIC_HPP__
#define __METRIC_HPP__

#include <Arduino.h>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

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
  virtual void add(T s) {
    T delta = s - avg;
    countS++;
    avg += delta / countS;
    var += delta * (s - avg);
    minS = (countS == 1) ? s : min(s, minS);
    maxS = (countS == 1) ? s : max(s, maxS);
    last = s;
  }

  virtual T minimum() const {
    return minS;
  }

  virtual T maximum() const {
    return maxS;
  }

  virtual T average() const {
    return avg;
  }

  virtual T variance() const {
    return (countS > 1) ? (var / (countS - 1)) : ((T) 0);
  }

  virtual T value() const {
    return last;
  }

  virtual String toJSON() const {
    String json = "{";
    json += "\"val\":" + String(this->value(), 2);
    json += ",\"avg\":" + String(this->average(), 2);
    json += ",\"var\":" + String(this->variance(), 2);
    json += ",\"min\":" + String(this->minimum(), 2);
    json += ",\"max\":" + String(this->maximum(), 2);
    json += "}";
    return json;
  }
};

template<typename T, uint16_t smoothingSize>
class SmoothMetric: public Metric<T> {
private:
  T movingAvg = (T) 0;
  T samples[smoothingSize];
  uint16_t index = 0;

public:
  SmoothMetric(): Metric<T>() {
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

  T value() const {
    return movingAvg;
  }
};

#endif
