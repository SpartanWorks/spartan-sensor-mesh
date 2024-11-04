#include "SSN.hpp"

SSN::SSN(uint16_t interval):
    sampleInterval(interval),
    ramFree(nullptr),
    fsFree(nullptr)
{}

SSN::~SSN() {
  if (this->ramFree) delete this->ramFree;
  if (this->fsFree) delete this->fsFree;
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
    this->ramFree->add(ESP.getFreeHeap() * 100.0 / ESP.getHeapSize());
  }

  if (this->fsFree != nullptr) {
    this->fsFree->add(SSN::fsUsedBytes() * 100.0 / SSN::fsTotalBytes());
  }
}

void SSN::connect(Device *d) const {
  if (this->ramFree != nullptr) {
    d->attach(this->ramFree);
  }
  if (this->fsFree != nullptr) {
    d->attach(this->fsFree);
  }
}

uint32_t SSN::fsUsedBytes() {
#ifdef ESP32
  return LittleFS.usedBytes();
#endif
#ifdef ESP8266
  FSInfo info;
  LittleFS.info(info);
  return info.usedBytes;
#endif
}

uint32_t SSN::fsTotalBytes() {
#ifdef ESP32
  return LittleFS.totalBytes();
#endif
#ifdef ESP8266
  FSInfo info;
  LittleFS.info(info);
  return info.totalBytes;
#endif
}

uint32_t SSN::fsFreeBytes() {
  return SSN::fsTotalBytes() - SSN::fsUsedBytes();
}
