#include "Scheduler.hpp"

Task::Task(uint16_t id, Function f): pid(id), fun(f) {}

void Task::sleep(uint32_t time) {
  this->runTime += time;
}

String Task::toString() const {
  return "pid: " + String(this->pid) + ", run: " + String(this->runTime) + " ms";
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

uint16_t Scheduler::spawn(Function f) {
  uint16_t pid = this->lastPid;
  this->tasks = new List<Task*>(new Task(pid, f), this->tasks);
  this->lastPid++;
  return pid;
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

String Scheduler::monitor() const {
  String out = "Task monitor: (" + String(millis()) + "ms)\r\n";
  foreach<Task*>(this->tasks, [&out](Task *t) {
    out += t->toString() + "\r\n";
  });
  return out;
}
