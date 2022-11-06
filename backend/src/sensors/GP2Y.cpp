#include "GP2Y.hpp"

GP2Y::GP2Y(uint8_t rx, uint8_t tx):
    pm(nullptr),
    raw(nullptr)
{
  this->serial = new SoftwareSerial(rx, tx);
}

GP2Y::~GP2Y() {
  if (this->pm != nullptr) delete this->pm;
  if (this->raw != nullptr) delete this->raw;
}

GP2Y* GP2Y::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t rx = (int) conn["rx"];
  uint16_t tx = (int) conn["tx"];

  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "software-uart") {
    return nullptr;
  }

  GP2Y *gp2y = new GP2Y(rx, tx);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "pm10") {
      gp2y->pm = new Reading<float>(name, "GP2Y", type, new WindowedValue<float, SAMPLE_BACKLOG>("μg/m³", 0, 600), cfg);
    } else if (type == "raw") {
      gp2y->raw = new Reading<float>(name, "GP2Y", type, new WindowedValue<float, SAMPLE_BACKLOG>("counts", 0, 1024), cfg);
    }
  }

  return gp2y;
}

void GP2Y::begin(System &system) {
  this->serial->begin(GP2Y_BAUDRATE);
  this->read();

  system.device().attach(this);

  system.scheduler().spawn("sample GP2Y", 115,[&](Task *t) {
    system.log().debug("Sampling GP2Y sensor.");
    this->update();
    t->sleep(GP2Y_SAMPLE_INTERVAL);
  });
}

float GP2Y::getPM() {
  float result = (this->factor * (this->readValue * this->rawScale) + this->offset) * 1000.0;
  // Dust value can't be lower than 0, so if we get negative values it means the callibration parameters aren't ideal, so we keep the value as a baseline.
  if(result < 0) {
    this->baseline = -result;
  }
  return result + this->baseline;
}

bool GP2Y::read() {
  this->serial->write(HEADER);
  this->serial->write(MEASURE_RAW_ONGOING); // Faster than MEASURE_RAW
  this->serial->write((uint8_t)N_SAMPLES);

  uint32_t spin = 100;
  while(this->serial->available() < 1 && spin > 0) {
    delay(1);
    spin--;
  }
  if(spin == 0) {
    String error = "Waited longer than 100 ms for ACK.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  uint8_t ack = this->serial->read();
  if (ack != ACK) {
    delay(100);
    String error = "Received bad ACK.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);

    while(this->serial->available() > 0) {
      this->serial->read();
    }
    return false;
  }

  spin = N_SAMPLES + 5;
  while(this->serial->available() < 2 && spin > 0) {
    delay(10); // How long it takes for a single measurement.
    spin--;
  }
  if(spin == 0) {
    String error = String("Waited longer than ") + String((N_SAMPLES + 5) * 10) + "ms for data.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  uint8_t low = this->serial->read();
  uint8_t high = this->serial->read();
  uint16_t raw = (uint16_t)(high) << 8 | low;

  this->readValue = raw;

  // NOTE In case of a previous read error this will set the stage for the next reading.
  while(this->serial->available() > 0) {
    this->serial->read();
  }
  return true;
}

void GP2Y::update() {
  if (this->read()) {
     if (this->pm != nullptr) this->pm->add(this->getPM());
     if (this->raw != nullptr) this->raw->add(this->readValue);
  }
}

void GP2Y::connect(Device *d) const {
  if (this->pm != nullptr) d->attach(this->pm);
  if (this->raw != nullptr) d->attach(this->raw);
}
