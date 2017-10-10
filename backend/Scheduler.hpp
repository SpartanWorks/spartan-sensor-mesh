#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <functional>
#include "List.hpp"

typedef std::function<void(void)> Function;

struct Task {
  Function fun;

  Task(Function f);
};

class Scheduler {
private:
  List<Task> *tasks = nullptr;

public:
  Scheduler();
  ~Scheduler();

  void begin();
  void spawn(Function f);
  void run();
};

#endif
