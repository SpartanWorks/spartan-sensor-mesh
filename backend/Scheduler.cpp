#include "Scheduler.hpp"

Task::Task(Function f): fun(f) {}

void Task::sleep(uint32_t time) {
  this->runTime += time;
}

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {
  if (this->tasks != nullptr) {
    foreach<Task*>(this->tasks, [](Task *t) {
      delete t;
    });
    delete this->tasks;
  }
}

void Scheduler::begin() {}

void Scheduler::spawn(Function f) {
  this->tasks = new List<Task*>(new Task(f), this->tasks);
}

void Scheduler::run() {
  foreach<Task*>(this->tasks, [](Task *t) {
    uint32_t currTime = millis();
    if (t->runTime < currTime) {
      t->fun(t);
      t->sleep(millis() - currTime);
    }
  });
}
