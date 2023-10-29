#include "Device.hpp"

Device::Device(): Device("", "", "", "")
{}

Device::Device(String m, String p): Device(m, "", "", p)
{}

Device::Device(String m, String n, String p): Device(m, n, n, p) {
}

Device::Device(String m, String g, String n, String p): dModel(m), dName(n), dPass(p), dGroup(g), list(nullptr) {
#ifdef ESP32
  uint32_t chipId = (uint32_t)(ESP.getEfuseMac() >> 32);
#endif

#ifdef ESP8266
  uint32_t chipId = (uint32_t)ESP.getChipId();
#endif

  dName = (dName == "") ? String("SSN-") + String(chipId, HEX) : dName;
  dGroup = (dGroup == "") ? dName : dGroup;
}

Device::~Device() {
  if (this->list != nullptr) {
    delete this->list;
  }
}

void Device::attach(const Reading<float> *s) {
  list = new List<const Reading<float>*>(s, list);
}

void Device::attach(const Sensor *s) {
  s->connect(this);
}

String Device::model() const {
  return this->dModel;
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

  JSONVar readings;
  json["readings"] = readings;

  uint16_t i = 0;

  foreach<const Reading<float>*>(list, [&json, &i](const Reading<float> *r) {
    JSONVar reading = r->toJSONVar();
    json["readings"][i] = reading;
    i++;
  });

  return json;
}

String Device::toJSON() const {
  return JSON.stringify(this->toJSONVar());
}
