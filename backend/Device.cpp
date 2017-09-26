#include "Device.hpp"

Device::Device(String p): Device("", p, "") {
}

Device::Device(String n, String p): Device(n, p, n) {
}

Device::Device(String n, String p, String g): dName(n), dPass(p), dGroup(g), list(nullptr) {
  dName = (dName == "") ? String("Device-") + String(ESP.getChipId(), HEX) : dName;
  dGroup = (dGroup == "") ? dName : dGroup;
}

void Device::attach(Sensor *s) {
  list = new List<Sensor*>(s, list);
}

void Device::begin() {
  foreach<Sensor*>(list, [](Sensor *s) {
    s->begin();
  });
}

void Device::update() {
  foreach<Sensor*>(list, [](Sensor *s) {
    s->update();
  });
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

  foreach<Sensor*>(list, [&json](Sensor *s) {
    json += s->toJSON();
  });

  json += "]}";
  return json;
}
