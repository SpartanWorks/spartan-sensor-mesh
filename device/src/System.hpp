#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Log.hpp"
#include "Mesh.hpp"
#include "Scheduler.hpp"
#include "Device.hpp"
#include "Sensor.hpp"
#include "Reading.hpp"
#include "List.hpp"

#define STATS_INTERVAL 60000 // 60 seconds

class System {
 private:
  Log l;
  Mesh m;
  Device dev;
  Scheduler sched;

 public:
  System(Timestamp slice);

  bool begin(JSONVar& config);
  void run();
  void reset();

  Scheduler& scheduler();
  Device& device();
  Mesh& mesh();
  Log& log();
};

#endif
