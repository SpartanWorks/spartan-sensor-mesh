#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Scheduler.hpp"
#include "Device.hpp"
#include "Sensor.hpp"
#include "SensorHub.hpp"
#include "Reading.hpp"
#include "List.hpp"
#include "Log.hpp"

#define STATS_INTERVAL 60000 // 60 seconds

class System {
 private:
  Log l;
  Scheduler sched;
  Device dev;

 public:
  System(Timestamp slice);

  bool begin(JSONVar& config);
  void run();

  Scheduler& scheduler();
  Device& device();
  Log& log();
};

#endif
