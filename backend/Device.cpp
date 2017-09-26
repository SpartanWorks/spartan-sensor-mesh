#include "Device.hpp"

Device::Device(String n, String g): dName(n), dGroup(g)
{}

void Device::begin() {
}

void Device::update() {
}

String Device::model() const {
  return "generic";
}

String Device::name() const {
  return this->dName;
}

String Device::group() const {
  return this->dGroup;
}

String Device::toJSON() const {
  String json = "{";
  json += "\"model\":\"" + this->model() + "\"";
  json += ",\"name\":\"" + this->name() + "\"";
  json += ",\"group\":\"" + this->group() + "\"";
  json += "}";
  return json;
}
