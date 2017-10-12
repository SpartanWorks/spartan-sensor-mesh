#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include <Arduino.h>
#include "List.hpp"

class Task;

typedef std::function<void(Task*)> Function;

class Task {
private:
  uint32_t vTime;
  uint32_t rTime;
  uint8_t priority;
  uint16_t pid;
  Function fun;

  String toString() const;

public:
  Task(uint16_t pid, uint8_t pri, Function f, uint32_t init);
  void sleep(uint32_t time);

  friend class Scheduler;
};

class Scheduler {
private:
  uint16_t lastPid = 0;
  List<Task*> *tasks = nullptr;

  void reschedule();

public:
  Scheduler();
  ~Scheduler();

  void begin();
  uint16_t spawn(uint8_t priority, Function f);
  void run();
  String monitor() const;
};

#endif
