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

const uint8_t MAX_PRIORITY = 0xFF;

class Scheduler;

class Task {
private:
  Scheduler *scheduler;
  TaskState state = RUNNING;
  uint8_t priority;
  Function fun;
  uint64_t vTime = 0;
  uint64_t rTime = 0;

  String toString() const;
  void updateTime(uint64_t time);

public:
  Task(Scheduler *s, uint8_t pri, Function f);
  void sleep(uint64_t time);
  void kill();

  friend class Scheduler;
};

class Scheduler {
private:
  uint64_t prevTime = 0;
  uint32_t overflows = 0;
  List<Task*> *running = nullptr;
  List<Task*> *waiting = nullptr;

  void reschedule();
  void rescheduleWaiting();
  void wake(uint64_t time);

public:
  Scheduler();
  ~Scheduler();

  uint64_t now();
  void begin();
  Task* spawn(uint8_t priority, Function f);
  void run();
  String monitor() const;
};

#endif
