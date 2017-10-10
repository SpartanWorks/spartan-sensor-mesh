#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include <Arduino.h>
#include "List.hpp"

class Task;

typedef std::function<void(Task*)> Function;

class Task {
private:
  uint32_t runTime = 0;
  Function fun;

public:
  Task(Function f);
  void sleep(uint32_t time);

  friend class Scheduler;
};

class Scheduler {
private:
  List<Task*> *tasks = nullptr;

public:
  Scheduler();
  ~Scheduler();

  void begin();
  void spawn(Function f);
  void run();
};

#endif
