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
  uint16_t pid;
  Function fun;

public:
  Task(uint16_t pid, Function f);
  void sleep(uint32_t time);
  String toString() const;

  friend class Scheduler;
};

class Scheduler {
private:
  uint16_t lastPid = 0;
  List<Task*> *tasks = nullptr;

public:
  Scheduler();
  ~Scheduler();

  void begin();
  uint16_t spawn(Function f);
  void run();
  String monitor() const;
};

#endif
