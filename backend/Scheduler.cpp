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

void Scheduler::reschedule() {
  List<Task*> *t = this->running;

  while (t != nullptr && t->next != nullptr && t->next->item->vTime < t->item->vTime) {
    Task *n = t->next->item;
    t->next->item = t->item;
    t->item = n;
    t = t->next;
  }
}

void Scheduler::rescheduleWaiting() {
  List<Task*> *t = this->waiting;

  while (t != nullptr && t->next != nullptr && t->next->item->rTime < t->item->rTime) {
    Task *n = t->next->item;
    t->next->item = t->item;
    t->item = n;
    t = t->next;
  }
}

void Scheduler::wake(uint32_t time) {
  if (this->waiting == nullptr) {
    return;
  }

  List<Task*> *wake = this->waiting;

  if (wake->item->rTime > time) {
    return;
  }

  wake->item->state = RUNNING;

  this->waiting = this->waiting->next;
  wake->next = this->running;
  this->running = wake;
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

    case SLEEPING: {
        List<Task*> *sleeping = this->running;
        this->running = sleeping->next;
        sleeping->next = this->waiting;
        this->waiting = sleeping;
        this->rescheduleWaiting();
      }
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
