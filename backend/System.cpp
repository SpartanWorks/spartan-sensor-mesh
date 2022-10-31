#include "System.hpp"

System::System(String password, Timestamp slice):
    sched(Scheduler(slice)),
    dev(Device(password))
{}

void System::begin() {
  // CONSOLE
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Serial console initialized");

  sched.begin();
  Serial.println("Scheduler initialized");

  sched.spawn("system monitor", 125, [&](Task *t) {
    Serial.print("Free heap memory: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" B");
    Serial.println(sched.monitor());
    t->sleep(STATS_INTERVAL);
  });
}

void System::run() {
  sched.run();
}

Scheduler& System::scheduler() {
  return sched;
}

Device& System::device() {
  return dev;
}
