#ifndef __METRIC_HPP__
#define __METRIC_HPP__

#include <Arduino.h>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Metric {
protected:
  T minS = (T) 0;
  T maxS = (T) 0;
  T var = (T) 0;
  T last = (T) 0;
  T avg = (T) 0;
  uint32_t count = 0;

  virtual void updateAverage(T s) {
    T delta = s - avg;
    avg += delta / count;
    var += delta * (s - avg);
  }

public:
  virtual void add(T s) {
    count++;
    minS = (count == 1) ? s : min(s, minS);
    maxS = (count == 1) ? s : max(s, maxS);
    last = s;
    this->updateAverage(s);
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
    return (count > 1) ? (var / (count - 1)) : ((T) 0);
  }

  virtual T value() const {
    return last;
  }

  virtual uint32_t samples() const {
    return count;
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

template<typename T, uint16_t windowSize>
class WindowedMetric: public Metric<T> {
protected:
  T window[windowSize];
  uint16_t index = 0;

  void updateAverage(T s) {
    if (this->count > windowSize) {
      T oldAvg = this->avg;
      this->avg += s/windowSize - window[index]/windowSize;
      this->var += (s - oldAvg) * (s - this->avg) - (window[index] - oldAvg) * (window[index] - this->avg);
    } else {
      Metric<T>::updateAverage(s);
    }
  }

public:
  WindowedMetric(): Metric<T>() {
    for(uint16_t i = 0; i < windowSize; ++i) {
      window[i] = (T) 0;
    }
  }

  void add(T s) {
    Metric<T>::add(s);
    window[index] = s;
    index = (index + 1) % windowSize;
  }

  virtual T variance() const {
    if (this->count > windowSize) {
      return this->var / (windowSize - 1);
    } else {
      return Metric<T>::variance();
    }
  }

};

#endif
