#include "Scheduler.hpp"

Task::Task(Scheduler *s, Priority p, Function f): scheduler(s), priority(p), fun(f) {}

void Task::sleep(Timestamp ms) {
  this->state = SLEEPING;
  this->wTime = this->scheduler->now() + ms * 1000;
}

void Task::kill() {
  this->state = KILLED;
}

void Task::updateTime(Timestamp time) {
#ifdef PROCESS_MONITOR
  this->rTime += time;
#endif
  this->vTime += time * this->priority;
}

Scheduler::Scheduler(): Scheduler(0) {}

Scheduler::Scheduler(Timestamp slice): timeSlice(slice) {}

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

Timestamp Scheduler::now() {
  Timestamp t = micros();

  if (t < this->prevTime) {
    this->overflows++;
  }
  this->prevTime = t;
  return t + this->overflows * 0x100000000ULL;
}

void Scheduler::begin() {}

Timestamp Scheduler::minVTime() {
  return (this->running != nullptr) ? this->running->item->vTime : 0;
}

Task* Scheduler::spawn(Priority priority, Function f) {
  Task *pid = new Task(this, priority, f);
  pid->vTime = this->minVTime(); // Not to starve other running processes.

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

  while (t != nullptr && t->next != nullptr && t->next->item->wTime < t->item->wTime) {
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

void Scheduler::wake(Timestamp time) {
  while (this->waiting != nullptr && this->waiting->item->wTime < time) {
    Task *t = this->waiting->item;
    t->state = RUNNING;
    t->wTime = 0;
    t->vTime = this->minVTime(); // Not to starve other running processes.

    moveHead(&this->waiting, &this->running);
    this->reschedule();
  }
}

void Scheduler::run() {
  this->wake(now());

  if (this->running == nullptr) {
    return;
  }

  Task *t = this->running->item;

  Timestamp start = now();
  Timestamp delta = 0;
  do {
    t->fun(t);
    delta = now() - start;
  } while(t->state == RUNNING && delta < this->timeSlice);

  t->updateTime(delta);

  switch (t->state) {
    case RUNNING:
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

#ifdef PROCESS_MONITOR

char buf[21];

String uint64String(Timestamp num) {
  const char map[] = "0123456789";
  char *p = &buf[21];
  *p = 0;

  do {
    *(--p) = map[num % 10];
    num /= 10;
  } while (num != 0);

  return String(p);
}

char output[160];

String Scheduler::taskToString(Task *t, Timestamp delta) {
  Timestamp cpu = (t->rTime - t->prevRTime) * 10000 / delta;

  sprintf(output, "%-10s %-5d %-3d %-10s %-21s %-21s %-21s\r\n",
          String((size_t) t, HEX).c_str(),
          t->priority,
          t->state,
          String(((uint32_t) cpu) / 100.0f, 2).c_str(),
          uint64String(t->rTime).c_str(),
          uint64String(t->vTime).c_str(),
          uint64String(t->wTime).c_str());

  String out = "";
  out += output;

  return out;
}

String Scheduler::monitor() {
  Timestamp currTime = this->now();
  Timestamp delta = currTime - this->monitorTime;
  Timestamp total = 0;
  this->monitorTime = currTime;

  String procs = "";
  Function f = [&procs, delta, this, &total](Task *t) {
    procs += this->taskToString(t, delta);
    total += t->rTime - t->prevRTime;
    t->prevRTime = t->rTime;
  };

  foreach<Task*>(this->running, f);
  foreach<Task*>(this->waiting, f);

  String out = "";
  sprintf(output, "%-10s %-5s %-3s %-10s %-21s %-21s %-21s\r\n",
          "PID",
          "PRI",
          "S",
          "% CPU",
          "RTIME",
          "VTIME",
          "WTIME");
  out += output;

  Timestamp cpu = (delta - total) * 10000 / delta;
  sprintf(output, "%-10s %-5d %-3d %-10s %-21s %-21s %-21s\r\n",
          "system",
          0,
          0,
          String(((uint32_t) cpu) / 100.0f, 2).c_str(),
          uint64String(currTime).c_str(),
          "0",
          "0");
  out += output;
  out += procs;

  return out;
}

#endif
