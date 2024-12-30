#include "ZE25O3.hpp"

ZE25O3::ZE25O3(uint8_t rx, uint8_t tx, uint16_t interval):
    serial(nullptr),
    sensor(nullptr),
    ozone(nullptr),
    sampleInterval(interval)
{
  this->serial = new SoftwareSerial(rx, tx);
  this->sensor = new ZE27(*(this->serial));
}

ZE25O3::ZE25O3(HardwareSerial &hw, uint16_t interval):
    serial(nullptr),
    sensor(nullptr),
    ozone(nullptr),
    sampleInterval(interval)
{
  this->sensor = new ZE27(hw);
}

ZE25O3::~ZE25O3() {
  if (this->ozone != nullptr) delete this->ozone;
  if (this->serial != nullptr) delete this->serial;
  if (this->sensor != nullptr) delete this->sensor;
}

ZE25O3* ZE25O3::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || (bus != "hardware-uart" && bus != "software-uart") ) {
    return nullptr;
  }

  ZE25O3 *ze25 = nullptr;

  if (bus == "software-uart") {
    uint16_t rx = (int) conn["rx"];
    uint16_t tx = (int) conn["tx"];

    ze25 = new ZE25O3(rx, tx, interval);
  } else if (bus == "hardware-uart") {
    uint16_t number = (int) conn["number"];
    switch(number) {
#ifdef ESP32
      case 2: {
        HardwareSerial& serial(Serial2);
        ze25 = new ZE25O3(serial, interval);
      }
        break;
#endif

      case 1: {
        HardwareSerial& serial(Serial1);
        ze25 = new ZE25O3(serial, interval);
      }
        break;

      case 0:
      default: {
        HardwareSerial& serial(Serial);
        ze25 = new ZE25O3(serial, interval);
      }
        break;
    }
  } else {
    return nullptr;
  }

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "ozone") {
      ze25->ozone = new Reading<float>(name, "ZE25-O3", type, new WindowedValue<float>(window, "ppm", 0, 10), cfg);
    }
  }

  return ze25;
}

void ZE25O3::begin(System &system) {
  this->sensor->begin(kQuestionAnswer);

  system.device().attach(this);

  system.scheduler().spawn("sample ZE25-O3", 115, [&](Task *t) {
    system.log().debug("Sampling ZE25-O3 sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void ZE25O3::update() {
  if (this->ozone != nullptr) {
    this->ozone->add(this->sensor->getPPM());
  }
}

void ZE25O3::connect(Device *d) const {
  if (this->ozone != nullptr) {
    d->attach(this->ozone);
  }
}
