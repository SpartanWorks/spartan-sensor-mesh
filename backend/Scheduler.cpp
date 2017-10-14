#include "Scheduler.hpp"

uint64_t now() {
  static uint64_t prevTime = 0;
  static uint32_t overflows = 0;

  uint64_t t = millis();

  if (t < prevTime) {
    overflows++;
  }
  prevTime = t;
  return t + overflows * 0xFFFFFFFF;
}

String uint64String(uint64_t num) {
  const char map[] = "0123456789";
  char buf[21];
  char *p = &buf[21];
  *p = 0;

  do {
    *(--p) = map[num % 10];
    num /= 10;
  } while (num != 0);

  return String(buf);
}

Task::Task(uint8_t p, Function f): priority(p), fun(f) {}

void Task::sleep(uint64_t ms) {
  this->state = SLEEPING;
  this->updateTime(now() + ms);
}

void Task::kill() {
  this->state = KILLED;
}

void Task::updateTime(uint64_t time) {
  this->rTime = time;
  this->vTime = time * this->priority / MAX_PRIORITY;
}

String Task::toString() const {
  return "pid: 0x" + String((size_t) this, HEX) + ", " +
      "state: " + String(this->state) + ", " +
      "priority: " + String(this->priority) + ", " +
      "real: " + uint64String(this->rTime) + " ms, " +
      "virtual: " + uint64String(this->vTime) + " ms";
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

void Scheduler::wake(uint64_t time) {
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
  this->wake(now());

  if (this->running == nullptr) {
    return;
  }

  Task *t = this->running->item;

  t->fun(t);

  switch (t->state) {
    case RUNNING:
      t->updateTime(now());
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
  String out = "Task monitor (" + uint64String(now()) + "ms):";
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
