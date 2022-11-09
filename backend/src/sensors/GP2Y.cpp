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
      gp2y->raw = new Reading<float>(name, "GP2Y", type, new WindowedValue<float, SAMPLE_BACKLOG>("counts", 0, MAX_RAW_READING_VALUE), cfg);
    }
  }

  return gp2y;
}

void GP2Y::begin(System &system) {
  this->serial->begin(GP2Y_BAUDRATE);
  this->init();
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

bool GP2Y::init() {
  if(!this->send(RESET)) {
    String error = "Failed to reset the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  delay(100);

  if(!this->send(INIT, NUM_CHANNELS)) {
    String error = "Failed to initialize the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  if(!this->send(INIT, NUM_CHANNELS)) {
    String error = "Failed to initialize the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  if(!this->send(SET_SAMPLING_TIME, CHANNEL, SAMPLE_TIME)
     || !this->send(SET_DELTA_TIME, CHANNEL, DELTA_TIME)
     || !this->send(SET_SAMPLING_INTERVAL, CHANNEL, SAMPLE_INTERVAL)){
    String error = "Failed to set sampling rate of the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }
  return true;
}

bool GP2Y::read() {
  this->flush();

  if(!this->send(MEASURE_RUNNING, CHANNEL)) {
    String error = "Didn't receive a valid ACK in more than 100 ms.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
  }

  if(!this->readData()) {
    String error = "Didn't receive data in more than 100 ms.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return false;
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

bool GP2Y::send(uint8_t command) {
  this->serial->write(HEADER);
  this->serial->write(command);
  return this->ack();
}

bool GP2Y::send(uint8_t command, uint8_t channel) {
  this->serial->write(HEADER);
  this->serial->write(command);
  this->serial->write(channel);
  return this->ack();
}

bool GP2Y::send(uint8_t command, uint8_t channel, uint16_t value) {
  uint8_t low = value & 0xFF;
  uint8_t high = value >> 8;

  this->serial->write(HEADER);
  this->serial->write(command);
  this->serial->write(channel);
  this->serial->write(low);
  this->serial->write(high);
  return this->ack();
}

bool GP2Y::await(uint16_t time, uint8_t numBytes) {
  uint16_t spin = time;

  while(this->serial->available() < numBytes && spin > 0) {
    delay(1);
    spin--;
  }
  if(spin == 0) {
    return false;
  }
  return true;
}

bool GP2Y::ack() {
  if(!this->await(100, 1)) {
    return false;
  }

  uint8_t ack = this->serial->read();
  if (ack != ACK) {
    delay(100);
    this->flush();
    return false;
  }

  return true;
}

bool GP2Y::readData() {
  if(!this->await(100, 2)) {
    return false;
  }

  uint8_t low = this->serial->read();
  uint8_t high = this->serial->read();
  uint16_t raw = (uint16_t)(high) << 8 | low;

  this->readValue = raw;

  return true;
}

void GP2Y::flush() {
  // NOTE In case of a previous read error this will set the stage for the next reading.
  while(this->serial->available() > 0) {
    this->serial->read();
  }
}
