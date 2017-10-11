#include "Scheduler.hpp"

Task::Task(uint16_t id, Function f, uint32_t init): pid(id), fun(f), rTime(init), vTime(init) {}

void Task::sleep(uint32_t time) {
  this->rTime += time;
  this->vTime += time;
}

String Task::toString() const {
  return "pid: " + String(this->pid) + "," +
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

uint16_t Scheduler::spawn(Function f) {
  uint16_t pid = this->lastPid;
  this->tasks = new List<Task*>(new Task(pid, f, millis()), this->tasks);
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
  uint32_t currTime = millis();

  List<Task*> *first = this->tasks;
  if (first == nullptr) {
    return;
  }

  Task *t = first->item;
  if (t->rTime > currTime) {
    return;
  }

  uint32_t delta = 0;
  do {
    t->fun(t);
    delta = millis() - currTime;
  } while(t->rTime + delta <= currTime);

  t->sleep(delta);
  this->reschedule();
}

String Scheduler::monitor() const {
  String out = "Task monitor (" + String(millis()) + "ms):\r\n";
  foreach<Task*>(this->tasks, [&out](Task *t) {
    out += " - " + t->toString() + "\r\n";
  });
  return out;
}
