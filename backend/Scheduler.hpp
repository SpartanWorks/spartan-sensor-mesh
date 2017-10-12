#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include <Arduino.h>
#include "List.hpp"

class Task;

typedef std::function<void(Task*)> Function;

enum TaskState {
  RUNNING = 0,
  SLEEPING,
  KILLED
};

class Task {
private:
  uint16_t pid;
  TaskState state = RUNNING;
  uint8_t priority;
  Function fun;
  uint32_t vTime = 0;
  uint32_t rTime = 0;

  String toString() const;
  void updateTime(uint32_t time);

public:
  Task(uint16_t pid, uint8_t pri, Function f);
  void sleep(uint32_t time);
  void kill();

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
