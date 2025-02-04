#include "SSN.hpp"

SSN::SSN(uint16_t interval):
    sampleInterval(interval),
    ramFree(nullptr),
    fsFree(nullptr),
    uptime(nullptr)
{}

SSN::~SSN() {
  if (this->ramFree) delete this->ramFree;
  if (this->fsFree) delete this->fsFree;
  if (this->uptime) delete this->uptime;
}

SSN* SSN::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar readings = config["readings"];

  SSN *ssn = new SSN(interval);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "memory-free") {
      ssn->ramFree = new Reading<float>(name, "SSN", type, new WindowedValue<float>(window, "%", 0, 100), cfg);
    } else if (type == "filesystem-free") {
      ssn->fsFree = new Reading<float>(name, "SSN", type, new WindowedValue<float>(window, "%", 0, 100), cfg);
    } else if (type == "uptime") {
      ssn->uptime = new Reading<float>(name, "SSN", type, new WindowedValue<float>(window, "s", 0, 31536000), cfg);
    }
  }

  return ssn;
}

void SSN::begin(System &system) {
  system.device().attach(this);

  system.scheduler().spawn("sample SSN", 115,[&](Task *t) {
    system.log().debug("Sampling SSN sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void SSN::update() {
  if (this->ramFree != nullptr) {
    this->ramFree->add(SSN::freeHeapSize() * 100.0 / SSN::totalHeapSize());
  }

  if (this->fsFree != nullptr) {
    this->fsFree->add(SSN::freeFSSize() * 100.0 / SSN::totalFSSize());
  }

  if (this->uptime != nullptr) {
    this->uptime->add(millis() / 1000);
  }
}

void SSN::connect(Device *d) const {
  if (this->ramFree != nullptr) {
    d->attach(this->ramFree);
  }
  if (this->fsFree != nullptr) {
    d->attach(this->fsFree);
  }
  if (this->uptime != nullptr) {
    d->attach(this->uptime);
  }
}

size_t SSN::usedFSSize() {
#ifdef ESP32
  return LittleFS.usedBytes();
#endif
#ifdef ESP8266
  FSInfo info;
  LittleFS.info(info);
  return info.usedBytes;
#endif
}

size_t SSN::totalFSSize() {
#ifdef ESP32
  return LittleFS.totalBytes();
#endif
#ifdef ESP8266
  FSInfo info;
  LittleFS.info(info);
  return info.totalBytes;
#endif
}

size_t SSN::freeFSSize() {
  return SSN::totalFSSize() - SSN::usedFSSize();
}

size_t SSN::freeHeapSize() {
return ESP.getFreeHeap();
}

size_t SSN::totalHeapSize() {
#ifdef ESP32
  return ESP.getHeapSize();
#endif
#ifdef ESP8266
  return ESP.getMaxFreeBlockSize();
#endif
}
