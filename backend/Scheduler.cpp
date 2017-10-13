#include "Scheduler.hpp"

Task::Task(uint8_t p, Function f): priority(p), fun(f) {}

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
  return "pid: 0x" + String((size_t) this, HEX) + ", " +
      "state: " + String(this->state) + ", " +
      "priority: " + String(this->priority) + ", " +
      "real: " + String(this->rTime) + " ms, " +
      "virtual: " + String(this->vTime) + " ms";
}

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {
  if (this->running != nullptr) {
    foreach<Task*>(this->running, [](Task *t) {
      delete t;
    });
    delete this->running;
  }
  if (this->waiting != nullptr) {
    foreach<Task*>(this->waiting, [](Task *t) {
      delete t;
    });
    delete this->waiting;
  }
}

void Scheduler::begin() {}

Task* Scheduler::spawn(uint8_t priority, Function f) {
  Task *pid = new Task(priority, f);
  this->running = new List<Task*>(pid, this->running);
  return pid;
}

inline void pushBack(List<Task*> *list) {
  Task *n = list->next->item;
  list->next->item = list->item;
  list->item = n;
}

void Scheduler::reschedule() {
  List<Task*> *t = this->running;

  while (t != nullptr && t->next != nullptr && t->next->item->vTime < t->item->vTime) {
    pushBack(t);
    t = t->next;
  }
}

void Scheduler::rescheduleWaiting() {
  List<Task*> *t = this->waiting;

  while (t != nullptr && t->next != nullptr && t->next->item->rTime < t->item->rTime) {
    pushBack(t);
    t = t->next;
  }
}

inline void moveHead(List<Task*> **from, List<Task*> **to) {
  List<Task*> *head = *from;
  *from = head->next;
  head->next = *to;
  *to = head;
}

void Scheduler::wake(uint32_t time) {
  if (this->waiting == nullptr) {
    return;
  }

  Task *t = this->waiting->item;

  if (t->rTime > time) {
    return;
  }

  t->state = RUNNING;
  moveHead(&this->waiting, &this->running);
}

void Scheduler::run() {
  this->wake(millis());

  if (this->running == nullptr) {
    return;
  }

  Task *t = this->running->item;

  t->fun(t);

  switch (t->state) {
    case RUNNING:
      t->updateTime(millis());
      this->reschedule();
      break;

    case SLEEPING:
      moveHead(&this->running, &this->waiting);
      this->rescheduleWaiting();
      break;

    case KILLED: {
        List<Task*> *killed = this->running;
        this->running = killed->next;
        killed->next = nullptr;
        delete killed->item;
        delete killed;
      }
      break;
  }
}

String Scheduler::monitor() const {
  String out = "Task monitor (" + String(millis()) + "ms):";
  out += "\r\nRunning tasks:\r\n";
  foreach<Task*>(this->running, [&out](Task *t) {
    out += " - " + t->toString() + "\r\n";
  });
  out += "Waiting tasks:\r\n";
  foreach<Task*>(this->waiting, [&out](Task *t) {
    out += " - " + t->toString() + "\r\n";
  });
  return out;
}
