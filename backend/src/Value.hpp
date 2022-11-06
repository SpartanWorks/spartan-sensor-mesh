#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template<typename T>
class Value {
protected:
  String sUnit;
  T rMin;
  T rMax;
  T var;
  T lastS;
  T meanS;
  uint32_t count;

  virtual void updateMean(T s) {
    T delta = s - meanS;
    meanS += delta / count;
    var += delta * (s - meanS);
  }

public:
  Value(String unit, T rMin, T rMax):
      sUnit(unit),
      rMin(rMin),
      rMax(rMax),
      var((T) 0),
      lastS((T) 0),
      meanS((T) 0),
      count(0)
  {}

  virtual ~Value() {}

  virtual void add(T s) {
    lastS = s;
    count++;
    this->updateMean(s);
  }

  virtual T mean() const {
    return meanS;
  }

  virtual T variance() const {
    return (count > 1) ? max(0, var / (count - 1)) : ((T) 0);
  }

  virtual T last() const {
    return lastS;
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
    json["last"] = this->last();
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
class MinMaxValue: public Value<T> {
protected:
  T minS;
  T maxS;

public:
  MinMaxValue(String unit, T rMin, T rMax):
      Value<T>(unit, rMin, rMax),
      minS((T) 0),
      maxS((T) 0)
  {}

  virtual void add(T s) {
    Value<T>::add(s);
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
    json["last"] = this->last();
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
class WindowedValue: public MinMaxValue<T> {
protected:
  T window[windowSize];
  uint16_t index;

  virtual void updateMean(T s) {
    uint16_t limit = this->samples();
    T m = (T) 0;
    for(uint16_t i = 0; i < limit; i++) {
      m += window[i];
    }
    this->meanS = m / limit;

    T v = (T) 0;
    for(uint16_t i = 0; i < limit; i++) {
      T delta = window[i] - this->meanS;
      v += delta * delta;
    }
    this->var = v / limit;
  }

public:
  WindowedValue(String unit, T rMin, T rMax):
      MinMaxValue<T>(unit, rMin, rMax),
      index(0)
  {
    for(uint16_t i = 0; i < windowSize; ++i) {
      window[i] = (T) 0;
    }
  }

  virtual void add(T s) {
    window[index] = s;
    index = (index + 1) % windowSize;
    MinMaxValue<T>::add(s);
  }

  virtual T variance() const {
    return this->var;
  }

  virtual uint32_t samples() const {
    return min(this->count, windowSize);
  }
};

#endif
