#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include <Arduino.h>
#include "List.hpp"

#define PROCESS_MONITOR

#ifdef PROCESS_MONITOR
  #include <stdio.h>
#endif

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
  Timestamp wTime = 0;

  #ifdef PROCESS_MONITOR
    Timestamp prevRTime = 0;
    Timestamp rTime = 0;
  #endif

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

  Timestamp timeSlice = 0;
  List<Task*> *running = nullptr;
  List<Task*> *waiting = nullptr;

  void reschedule();
  void rescheduleWaiting();
  void wake(Timestamp time);
  Timestamp minVTime();

  #ifdef PROCESS_MONITOR
    String taskToString(Task *t, Timestamp delta);
    Timestamp monitorTime = 0;
  #endif

public:
  Scheduler();
  Scheduler(Timestamp slice);
  ~Scheduler();

  Timestamp now();
  void begin();
  Task* spawn(Priority priority, Function f);
  void run();

  #ifdef PROCESS_MONITOR
    String monitor();
  #endif
};

#endif
