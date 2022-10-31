#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <Arduino.h>
#include "Scheduler.hpp"
#include "Device.hpp"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Reading.hpp"
#include "List.hpp"

#define STATS_INTERVAL 60000 // 60 seconds

class System {
 private:
  Scheduler sched;
  Device dev;

 public:
  System(String password, Timestamp slice);

  void begin();
  void run();

  Scheduler& scheduler();
  Device& device();
};

#endif
