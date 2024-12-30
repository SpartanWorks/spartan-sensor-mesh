#include "ZE25O3.hpp"

ZE25O3::ZE25O3(UART *s, uint16_t interval):
    ozone(nullptr),
    serial(s),
    sampleInterval(interval)
{}

ZE25O3::~ZE25O3() {
  if (this->ozone != nullptr) delete this->ozone;
  if (this->serial != nullptr) delete this->serial;
}

ZE25O3* ZE25O3::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  JSONVar readings = config["readings"];

  UART *serial = UART::create(conn);

  if(conn == undefined || readings == undefined || serial == nullptr) {
    return nullptr;
  }

  ZE25O3 *ze25 = new ZE25O3(serial, interval);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "ozone") {
      ze25->ozone = new Reading<float>(name, "ZE25-O3", type, new WindowedValue<float>(window, "ppb", 0, 10000), cfg);
    }
  }

  return ze25;
}

void ZE25O3::begin(System &system) {
  this->initSensor();

  system.device().attach(this);

  system.scheduler().spawn("sample ZE25-O3", 115, [&](Task *t) {
    system.log().debug("Sampling ZE25-O3 sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void ZE25O3::update() {
  if (this->ozone != nullptr) {
    this->ozone->add(this->readSensor());
  }
}

void ZE25O3::connect(Device *d) const {
  if (this->ozone != nullptr) {
    d->attach(this->ozone);
  }
}

uint8_t QA_MODE[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
uint8_t REQ_DATA[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

void ZE25O3::initSensor() {
  this->serial->begin(BAUD_RATE);
  this->serial->getStream()->write(QA_MODE, sizeof(QA_MODE));
  this->serial->getStream()->flush();
}

uint8_t checksum(const uint8_t *buf, uint8_t len) {
  uint8_t sum = 0;

  for (uint8_t i = 1; i < len - 1; i++) {
    sum += buf[i];
  }

  return (~sum)+1;
}

float ZE25O3::readSensor() {
  this->serial->getStream()->write(REQ_DATA, sizeof(REQ_DATA));
  this->serial->getStream()->flush();

  uint8_t buf[RESP_LEN];
  this->serial->getStream()->readBytes(buf, RESP_LEN);

  if(buf[0] == 0xFF && buf[1] == 0x86 && buf[8] == checksum(buf, RESP_LEN)) {
    return (float) (buf[2] * 256 + buf[3]);
  } else {
    // NOTE Error reading the data.
    return -1;
  }
}
