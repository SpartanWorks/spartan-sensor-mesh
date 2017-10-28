#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include <Arduino.h>
#include "List.hpp"

class Task;

typedef std::function<void(Task*)> Function;
typedef uint8_t Priority;
typedef uint64_t Timestamp;

enum TaskState {
  RUNNING = 0,
  SLEEPING,
  KILLED
};

class Scheduler;

class Task {
private:
  Scheduler *scheduler;
  TaskState state = RUNNING;
  Priority priority;
  Function fun;
  Timestamp vTime = 0;
  Timestamp rTime = 0;
  Timestamp wTime = 0;

  String toString() const;
  void updateTime(Timestamp time);

public:
  Task(Scheduler *s, Priority pri, Function f);
  void sleep(Timestamp time);
  void kill();

  friend class Scheduler;
};

class Scheduler {
private:
  Timestamp prevTime = 0;
  uint32_t overflows = 0;
  List<Task*> *running = nullptr;
  List<Task*> *waiting = nullptr;

  void reschedule();
  void rescheduleWaiting();
  void wake(Timestamp time);

public:
  Scheduler();
  ~Scheduler();

  Timestamp now();
  void begin();
  Task* spawn(Priority priority, Function f);
  void run();
  String monitor();
};

#endif
