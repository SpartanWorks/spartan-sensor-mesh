#include "Device.hpp"

Device::Device(String p): Device("", p, "") {
}

Device::Device(String n, String p): Device(n, p, n) {
}

Device::Device(String n, String p, String g): dName(n), dPass(p), dGroup(g), list(nullptr) {
  uint32_t chipId = (uint32_t)(ESP.getEfuseMac() >> 32);
  dName = (dName == "") ? String("Device-") + String(chipId, HEX) : dName;
  dGroup = (dGroup == "") ? dName : dGroup;
}

Device::~Device() {
  if (this->list != nullptr) {
    delete this->list;
  }
}

void Device::attach(const Sensor<float> *s) {
  list = new List<const Sensor<float>*>(s, list);
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

JSONVar Device::toJSONVar() const {
  JSONVar json;
  json["model"] = this->model();
  json["name"] = this->name();
  json["group"] = this->group();

  JSONVar sensors;
  uint16_t i = 0;

  foreach<const Sensor<float>*>(list, [&sensors, &i](const Sensor<float> *s) {
    JSONVar sensor = s->toJSONVar();
    sensors[i] = sensor;
    i++;
  });

  json["sensors"] = sensors;

  return json;
}

String Device::toJSON() const {
  return JSON.stringify(this->toJSONVar());
}
