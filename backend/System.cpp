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

  // FILE SYSTEM
  SPIFFS.begin();
  Serial.println("File system initialized (" + String(SPIFFS.usedBytes()) + " B / " + String(SPIFFS.totalBytes()) + " B):");

  File dir = SPIFFS.open("/");
  if(!dir.isDirectory()) {
    Serial.println("/ is not a directory!");
  } else {
    File f;
    while (f = dir.openNextFile()) {
      Serial.print("- " + String(f.name()));
      Serial.println(" (" + String(f.size()) + " B)");
      f.close();
    }
  }
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
