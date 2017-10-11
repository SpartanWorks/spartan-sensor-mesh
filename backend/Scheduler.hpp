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
  uint16_t pid;
  Function fun;

  String toString() const;

public:
  Task(uint16_t pid, Function f, uint32_t init);
  void sleep(uint32_t time);

  friend class Scheduler;
};

class Scheduler {
private:
  uint16_t lastPid = 0;
  List<Task*> *tasks = nullptr;

  List<Task*> *pop();
  void push(List<Task*> *t);

public:
  Scheduler();
  ~Scheduler();

  void begin();
  uint16_t spawn(Function f);
  void run();
  String monitor() const;
};

#endif
