#include "Scheduler.hpp"

Task::Task(Function f): fun(f) {}

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {
  if (this->tasks != nullptr) {
    delete this->tasks;
  }
}

void Scheduler::begin() {}

void Scheduler::spawn(Function f) {
  this->tasks = new List<Task>(Task(f), this->tasks);
}

void Scheduler::run() {
  foreach<Task>(this->tasks, [this](Task t) {
    t.fun();
  });
}
