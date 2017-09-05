#ifndef __STATS_H__
#define __STATS_H__

#include <Arduino.h>

template<typename T, uint32_t size>
class Stats {
private:
  T samples[size];
  uint32_t index = 0;
  T minS;
  T maxS;

public:
  Stats(T spanMin, T spanMax) {
    minS = spanMax;
    maxS = spanMin;
  }

  void add(T s) {
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

  T last() {
    uint32_t i = (index + size - 1) % size;
    return samples[i];
  }

  T average() {
    T avg;
    for(uint32_t i = 0; i < size; ++i) {
      avg += samples[i];
    }
    return avg / size;
  }
};

#endif
