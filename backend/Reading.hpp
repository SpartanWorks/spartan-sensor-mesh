#ifndef __READING_HPP__
#define __READING_HPP__

#include <Arduino.h>

#define DIGITS 5

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Reading {
protected:
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
    last = s;
    count++;
    this->updateMean(s);
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
    json += ",\"samples\":" + String(this->samples());
    json += "}";
    return json;
  }
};

template<typename T>
class MinMaxReading: public Reading<T> {
protected:
  T minS = (T) 0;
  T maxS = (T) 0;

public:
  virtual void add(T s) {
    Reading<T>::add(s);
    minS = (this->count == 1) ? s : min(s, minS);
    maxS = (this->count == 1) ? s : max(s, maxS);
  }

  virtual T minimum() const {
    return minS;
  }

  virtual T maximum() const {
    return maxS;
  }

  virtual String toJSON() const {
    String json = "{";
    json += "\"value\":" + String(this->value(), DIGITS);
    json += ",\"mean\":" + String(this->mean(), DIGITS);
    json += ",\"variance\":" + String(this->variance(), DIGITS);
    json += ",\"minimum\":" + String(this->minimum(), DIGITS);
    json += ",\"maximum\":" + String(this->maximum(), DIGITS);
    json += ",\"samples\":" + String(this->samples());
    json += "}";
    return json;
  }
};

template<typename T, uint16_t windowSize>
class WindowedReading: public Reading<T> {
protected:
  T window[windowSize];
  uint16_t index = 0;

  void updateMean(T s) {
    if (this->count > windowSize) {
      T oldMean = this->meanS;
      this->meanS += s/windowSize - window[index]/windowSize;
      this->var += (s - oldMean) * (s - this->meanS) - (window[index] - oldMean) * (window[index] - this->meanS);
    } else {
      Reading<T>::updateMean(s);
    }
  }

public:
  WindowedReading(): Reading<T>() {
    for(uint16_t i = 0; i < windowSize; ++i) {
      window[i] = (T) 0;
    }
  }

  void add(T s) {
    Reading<T>::add(s);
    window[index] = s;
    index = (index + 1) % windowSize;
  }

  virtual T variance() const {
    return (this->count > windowSize) ? (this->var / (windowSize - 1)) : Reading<T>::variance();
  }

  virtual uint32_t samples() const {
    return min(this->count, windowSize);
  }

  virtual String toJSON() const {
    String json = "{";
    json += "\"value\":" + String(this->value(), DIGITS);
    json += ",\"mean\":" + String(this->mean(), DIGITS);
    json += ",\"variance\":" + String(this->variance(), DIGITS);
    json += ",\"samples\":" + String(this->samples());
    json += "}";
    return json;
  }
};

#endif
