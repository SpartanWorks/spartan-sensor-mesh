#include "GP2Y.hpp"

GP2Y::GP2Y(uint8_t rx, uint8_t tx):
    sensor(ADCOverUART(rx, tx, NUM_CHANNELS)),
    pm(nullptr),
    raw(nullptr)
{}

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
  system.device().attach(this);

  if(!this->sensor.begin()
     || !this->sensor.setSamplingInterval(CHANNEL, SAMPLE_INTERVAL)
     || !this->sensor.setSamplingTime(CHANNEL, SAMPLE_TIME)
     || !this->sensor.setDeltaTime(CHANNEL, DELTA_TIME)) {
    String error = "Failed to initialize the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
    return;
  }

  system.scheduler().spawn("sample GP2Y", 115,[&](Task *t) {
    system.log().debug("Sampling GP2Y sensor.");
    this->update();
    t->sleep(GP2Y_SAMPLE_INTERVAL);
  });
}

float GP2Y::getPM() {
  float result = (this->factor * this->sensor.getVoltage(CHANNEL) + this->offset) * 1000.0;
  // Dust value can't be lower than 0, so if we get negative values it means the callibration parameters aren't ideal, so we keep the value as a baseline.
  if(result < 0) {
    this->baseline = -result;
  }
  return result + this->baseline;
}

void GP2Y::update() {
  if (this->sensor.read(CHANNEL)) {
     if (this->raw != nullptr) this->raw->add(this->sensor.getReading(CHANNEL));
     if (this->pm != nullptr) this->pm->add(this->getPM());
  } else {
    String error = "Failed to read data from the sensor.";
    if (this->pm != nullptr) this->pm->setError(error);
    if (this->raw != nullptr) this->raw->setError(error);
  }
}

void GP2Y::connect(Device *d) const {
  if (this->pm != nullptr) d->attach(this->pm);
  if (this->raw != nullptr) d->attach(this->raw);
}
