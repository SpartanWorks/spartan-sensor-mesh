#ifndef __METRIC_HPP__
#define __METRIC_HPP__

#include <Arduino.h>

#define DIGITS 5

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Metric {
protected:
  T minS = (T) 0;
  T maxS = (T) 0;
  T var = (T) 0;
  T last = (T) 0;
  T meanS = (T) 0;
  uint32_t count = 0;

  virtual void updateMean(T s) {
    T delta = s - meanS;
    meanS += delta / count;
    var += delta * (s - meanS);
  }

public:
  virtual void add(T s) {
    count++;
    minS = (count == 1) ? s : min(s, minS);
    maxS = (count == 1) ? s : max(s, maxS);
    last = s;
    this->updateMean(s);
  }

  virtual T minimum() const {
    return minS;
  }

  virtual T maximum() const {
    return maxS;
  }

  virtual T mean() const {
    return meanS;
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
    json += "\"value\":" + String(this->value(), DIGITS);
    json += ",\"mean\":" + String(this->mean(), DIGITS);
    json += ",\"variance\":" + String(this->variance(), DIGITS);
    json += ",\"minimum\":" + String(this->minimum(), DIGITS);
    json += ",\"maximum\":" + String(this->maximum(), DIGITS);
    json += "}";
    return json;
  }
};

template<typename T, uint16_t windowSize>
class WindowedMetric: public Metric<T> {
protected:
  T window[windowSize];
  uint16_t index = 0;

  void updateMean(T s) {
    if (this->count > windowSize) {
      T oldMean = this->meanS;
      this->meanS += s/windowSize - window[index]/windowSize;
      this->var += (s - oldMean) * (s - this->meanS) - (window[index] - oldMean) * (window[index] - this->meanS);
    } else {
      Metric<T>::updateMean(s);
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
    return (this->count > windowSize) ? (this->var / (windowSize - 1)) : Metric<T>::variance();
  }

  virtual String toJSON() const {
    String json = "{";
    json += "\"value\":" + String(this->value(), DIGITS);
    json += ",\"mean\":" + String(this->mean(), DIGITS);
    json += ",\"variance\":" + String(this->variance(), DIGITS);
    // NOTE Max & min don't make much sense here, so we don't return them.
    json += ",\"window\":" + String(windowSize);
    json += "}";
    return json;
  }
};

#endif
