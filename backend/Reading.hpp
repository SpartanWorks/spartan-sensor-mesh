#ifndef __READING_HPP__
#define __READING_HPP__

#include <Arduino.h>

#define DIGITS 5

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Reading {
protected:
  String sUnit = "";
  T rMin = (T) 0;
  T rMax = (T) 0;
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
  Reading(String unit, T rMin, T rMax):
      sUnit(unit),
      rMin(rMin),
      rMax(rMax)
  {}

  virtual void add(T s) {
    last = s;
    count++;
    this->updateMean(s);
  }

  virtual T mean() const {
    return meanS;
  }

  virtual T variance() const {
    return (count > 1) ? max(0, var / (count - 1)) : ((T) 0);
  }

  virtual T value() const {
    return last;
  }

  virtual String unit() const {
    return this->sUnit;
  }

  virtual T rangeMin() const {
    return this->rMin;
  }

  virtual T rangeMax() const {
    return this->rMax;
  }

  virtual uint32_t samples() const {
    return count;
  }

  virtual String toJSON() const {
    String json = "{";
    json += "\"value\":" + String(this->value(), DIGITS);
    json += ",\"unit\":\"" + this->unit() + "\"";
    json += ",\"stats\":{";
      json += ",\"mean\":" + String(this->mean(), DIGITS);
      json += ",\"variance\":" + String(this->variance(), DIGITS);
      json += ",\"samples\":" + String(this->samples());
    json += "}";
    json += ",\"range\":{";
      json += "\"minimum\":" + String(this->rangeMin(), DIGITS);
      json += ",\"maximum\":" + String(this->rangeMax(), DIGITS);
    json += "}";
    return json + "}";
  }
};

template<typename T>
class MinMaxReading: public Reading<T> {
protected:
  T minS = (T) 0;
  T maxS = (T) 0;

public:
  MinMaxReading(String unit, T rMin, T rMax):
      Reading<T>(unit, rMin, rMax)
  {}

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
    json += ",\"unit\":\"" + this->unit() + "\"";
    json += ",\"stats\":{";
      json += "\"mean\":" + String(this->mean(), DIGITS);
      json += ",\"variance\":" + String(this->variance(), DIGITS);
      json += ",\"samples\":" + String(this->samples());
      json += ",\"minimum\":" + String(this->minimum(), DIGITS);
      json += ",\"maximum\":" + String(this->maximum(), DIGITS);
    json += "}";
    json += ",\"range\":{";
      json += "\"minimum\":" + String(this->rangeMin(), DIGITS);
      json += ",\"maximum\":" + String(this->rangeMax(), DIGITS);
    json += "}";
    return json + "}";
  }
};

template<typename T, uint16_t windowSize>
class WindowedReading: public MinMaxReading<T> {
protected:
  T window[windowSize];
  uint16_t index = 0;

  virtual void updateMean(T s) {
    if (this->count > windowSize) {
      T oldMean = this->meanS;
      this->meanS += s/windowSize - window[index]/windowSize;
      this->var += (s - oldMean) * (s - this->meanS) - (window[index] - oldMean) * (window[index] - this->meanS);
    } else {
      MinMaxReading<T>::updateMean(s);
    }
  }

public:
  WindowedReading(String unit, T rMin, T rMax):
      MinMaxReading<T>(unit, rMin, rMax)
  {
    for(uint16_t i = 0; i < windowSize; ++i) {
      window[i] = (T) 0;
    }
  }

  virtual void add(T s) {
    MinMaxReading<T>::add(s);
    window[index] = s;
    index = (index + 1) % windowSize;
  }

  virtual T variance() const {
    return (this->count > windowSize) ? max(0, this->var / (windowSize - 1)) : MinMaxReading<T>::variance();
  }

  virtual uint32_t samples() const {
    return min(this->count, windowSize);
  }
};

#endif
