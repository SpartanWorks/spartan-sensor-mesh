#ifndef __READING_HPP__
#define __READING_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>

#define DIGITS 5

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Reading {
protected:
  String sUnit;
  T rMin;
  T rMax;
  T var;
  T last;
  T meanS;
  uint32_t count;

  virtual void updateMean(T s) {
    T delta = s - meanS;
    meanS += delta / count;
    var += delta * (s - meanS);
  }

public:
  Reading(String unit, T rMin, T rMax):
      sUnit(unit),
      rMin(rMin),
      rMax(rMax),
      var((T) 0),
      last((T) 0),
      meanS((T) 0),
      count(0)
  {}

  virtual ~Reading() {}

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

  virtual JSONVar toJSONVar() const {
    JSONVar json;
    json["value"] = this->value();
    json["unit"] = this->unit();

    JSONVar stats;
    stats["mean"] = this->mean();
    stats["variance"] = this->variance();
    stats["samples"] = (unsigned long) this->samples();
    json["stats"] = stats;

    JSONVar range;
    range["minimum"] = this->rangeMin();
    range["maximum"] = this->rangeMax();
    json["range"] = range;

    return json;
  }

  virtual String toJSON() const {
    return JSON.stringify(this->toJSONVar());
  }

};

template<typename T>
class MinMaxReading: public Reading<T> {
protected:
  T minS;
  T maxS;

public:
  MinMaxReading(String unit, T rMin, T rMax):
      Reading<T>(unit, rMin, rMax),
      minS((T) 0),
      maxS((T) 0)
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

  virtual JSONVar toJSONVar() const {
    JSONVar json;
    json["value"] = this->value();
    json["unit"] = this->unit();

    JSONVar stats;
    stats["mean"] = this->mean();
    stats["variance"] = this->variance();
    stats["samples"] = (unsigned long) this->samples();
    stats["minimum"] = this->minimum();
    stats["maximum"] = this->maximum();
    json["stats"] = stats;

    JSONVar range;
    range["minimum"] = this->rangeMin();
    range["maximum"] = this->rangeMax();
    json["range"] = range;

    return json;
  }
};

template<typename T, uint16_t windowSize>
class WindowedReading: public MinMaxReading<T> {
protected:
  T window[windowSize];
  uint16_t index;

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
      MinMaxReading<T>(unit, rMin, rMax),
      index(0)
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