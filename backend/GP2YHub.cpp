#include "GP2YHub.hpp"

GP2YHub::GP2YHub(uint8_t rx, uint8_t tx):
    pm(Sensor("GP2Y", "pm10", "PM 10", new WindowedReading<float, SAMPLE_BACKLOG>()))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void GP2YHub::begin() {
  this->serial->begin(GP2Y_BAUDRATE);
  this->read();
}

float GP2YHub::getPM() {
  return (this->factor * (this->readValue  * this->rawScale) + this->offset) * 1000.0; 
}

bool GP2YHub::read() {
  //Serial.println("Sending command");
  this->serial->write(HEADER);
  this->serial->write(MEASURE_RAW);
  this->serial->write(SAMPLING);

  //Serial.println("Awaiting ACK");
  uint32_t spin = 100;
  while(this->serial->available() < 1 && spin > 0) {
    delay(1);
    spin--;
  }
  if(spin == 0) {
    //Serial.println("Waited too long for ACK, aborting!");
    return false;
  }

  uint8_t ack = this->serial->read();
  //Serial.println("Got ACK");
  if (ack != ACK) {
    delay(10);
    //Serial.println("ACK was bad, dumping stuff.");
    while(this->serial->available() > 0) {
      this->serial->read();
    }
    return false;
  }

  //Serial.println("ACK was good, awaiting value.");
  spin = 100;
  while(this->serial->available() < 2 && spin > 0) {
    delay(1);
    spin--;
  }
  if(spin == 0) {
    //Serial.println("Waited too long for data, aborting!");
    return false;
  }

  uint8_t low = this->serial->read();
  uint8_t high = this->serial->read();
  uint16_t raw = (uint16_t)(high) << 8 | low;

  //Serial.print("Got value.");
  //Serial.println(raw);

  this->readValue = raw;
  
  //Serial.println("Dumping remaining garbage.");
  while(this->serial->available() > 0) {
    this->serial->read();
  }
  return true;
}

void GP2YHub::update() {
  if (this->read()) {
    this->pm.add(this->getPM());
  } else {
    this->pm.add(NAN);
  }
}

void GP2YHub::connect(Device *d) const {
  d->attach(&this->pm);
}
