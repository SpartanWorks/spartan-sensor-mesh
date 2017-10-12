#include "Scheduler.hpp"

Task::Task(uint16_t id, uint8_t p, Function f): pid(id), priority(p), fun(f) {}

void Task::sleep(uint32_t delta) {
  this->state = SLEEPING;
  this->updateTime(millis() + delta);
}

void Task::kill() {
  this->state = KILLED;
}

void Task::updateTime(uint32_t time) {
  this->rTime = time;
  this->vTime = time * this->priority;
}

String Task::toString() const {
  return "pid: " + String(this->pid) + ", " +
      "priority: " + String(this->priority) + ", " +
      "real: " + String(this->rTime) + " ms, " +
      "virtual: " + String(this->vTime) + " ms";
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

uint16_t Scheduler::spawn(uint8_t priority, Function f) {
  uint16_t pid = this->lastPid;
  this->tasks = new List<Task*>(new Task(pid, priority, f), this->tasks);
  this->reschedule();

  this->lastPid++;
  return pid;
}

void Scheduler::reschedule() {
  List<Task*> *t = this->tasks;

  while (t != nullptr && t->next != nullptr && t->next->item->vTime < t->item->vTime) {
    Task *n = t->next->item;
    t->next->item = t->item;
    t->item = n;
    t = t->next;
  }
}

void Scheduler::run() {
  if (this->tasks == nullptr) {
    return;
  }

  Task *t = this->tasks->item;
  if (t->rTime > millis()) {
    return;
  }

  t->fun(t);

  switch (t->state) {
    case RUNNING:
      t->updateTime(millis());

    case SLEEPING:
      ; // Do nothing.

    case KILLED:
      List<Task*> *killed = this->tasks;
      this->tasks = killed->next;
      killed->next = nullptr;
      delete killed->item;
      delete killed;
  }

  this->reschedule();
}

String Scheduler::monitor() const {
  String out = "Task monitor (" + String(millis()) + "ms):\r\n";
  foreach<Task*>(this->tasks, [&out](Task *t) {
    out += " - " + t->toString() + "\r\n";
  });
  return out;
}
