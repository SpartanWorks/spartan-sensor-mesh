#include "GP2YHub.hpp"

GP2YHub::GP2YHub(uint8_t rx, uint8_t tx):
    pm(Sensor<float>("Total PM", "GP2Y", "pm10", new WindowedReading<float, SAMPLE_BACKLOG>("μg/m³", 0, 600))),
    raw(Sensor<float>("Raw value", "GP2Y", "adc", new WindowedReading<float, SAMPLE_BACKLOG>("counts", 0, 1024)))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void GP2YHub::begin(System &system) {
  this->serial->begin(GP2Y_BAUDRATE);
  this->read();

  system.device().attach(this);

  system.scheduler().spawn("sample GP2Y", 115,[=](Task *t) {
    Serial.println("Sampling GP2Y hub.");
    this->update();
    t->sleep(GP2Y_SAMPLE_INTERVAL);
  });
}

float GP2YHub::getPM() {
  float result = (this->factor * (this->readValue * this->rawScale) + this->offset) * 1000.0;
  // Dust value can't be lower than 0, so if we get negative values it means the callibration parameters aren't ideal, so we keep the value as a baseline.
  if(result < 0) {
    this->baseline = -result;
  }
  return result + this->baseline;
}

bool GP2YHub::read() {
  this->serial->write(HEADER);
  this->serial->write(MEASURE_RAW_ONGOING); // Faster than MEASURE_RAW
  this->serial->write((uint8_t)N_SAMPLES);

  uint32_t spin = 100;
  while(this->serial->available() < 1 && spin > 0) {
    delay(1);
    spin--;
  }
  if(spin == 0) {
    this->pm.setError("Waited longer than 100 ms for ACK.");
    return false;
  }

  uint8_t ack = this->serial->read();
  if (ack != ACK) {
    delay(100);
    this->pm.setError("Received bad ACK.");

    while(this->serial->available() > 0) {
      this->serial->read();
    }
    return false;
  }

  uint32_t t0 = millis();
  spin = N_SAMPLES + 5;
  while(this->serial->available() < 2 && spin > 0) {
    delay(10); // How long it takes for a single measurement.
    spin--;
  }
  if(spin == 0) {
    this->pm.setError(String("Waited longer than ") + String((N_SAMPLES + 5) * 10) + "ms for data.");
    return false;
  }
  uint32_t t1 = millis();

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

void GP2YHub::update() {
  if (this->read()) {
    this->pm.add(this->getPM());
  }
  // NOTE read() already handles setting the error.

#ifdef GP2Y_RAW_READING
  this->raw.add(this->readValue);
#endif
}

void GP2YHub::connect(Device *d) const {
  d->attach(&this->pm);
#ifdef GP2Y_RAW_READING
  d->attach(&this->raw);
#endif
}
