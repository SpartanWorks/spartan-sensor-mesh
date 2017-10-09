#include "Device.hpp"

Device::Device(String p): Device("", p, "") {
}

Device::Device(String n, String p): Device(n, p, n) {
}

Device::Device(String n, String p, String g): dName(n), dPass(p), dGroup(g), list(nullptr) {
  dName = (dName == "") ? String("Device-") + String(ESP.getChipId(), HEX) : dName;
  dGroup = (dGroup == "") ? dName : dGroup;
}

Device::~Device() {
  if (this->list != nullptr) {
    delete this->list;
  }
}

void Device::attach(const Sensor *s) {
  list = new List<const Sensor*>(s, list);
}

void Device::attach(const SensorHub *s) {
  s->connect(this);
}

String Device::model() const {
  return "generic";
}

String Device::name() const {
  return this->dName;
}

String Device::password() const {
  return this->dPass;
}

String Device::group() const {
  return this->dGroup;
}

String Device::toJSON() const {
  String json = "{";
  json += "\"model\":\"" + this->model() + "\"";
  json += ",\"name\":\"" + this->name() + "\"";
  json += ",\"group\":\"" + this->group() + "\"";
  json += ",\"sensors\":[";

  bool first = true;
  foreach<const Sensor*>(list, [&json, &first](const Sensor *s) {
    if(!first) {
      json += ",";
    }
    json += s->toJSON();
    first = false;
  });

  json += "]}";
  return json;
}
