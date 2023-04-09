#include "ADCOverUART.hpp"

ADCOverUART::ADCOverUART(uint8_t rx, uint8_t tx, uint8_t numChannels):
    numChannels(numChannels)
{
  this->serial = new SoftwareSerial(rx, tx);
  this->results = new uint16_t[numChannels];
}

ADCOverUART::~ADCOverUART() {
  if (this->serial != nullptr) delete this->serial;
  if (this->results != nullptr) delete this->results;
}

bool ADCOverUART::begin() {
  this->serial->begin(ADC_OVER_UART_BAUDRATE);
  return this->send(INIT, this->numChannels);
}

bool ADCOverUART::reset() {
  return this->send(RESET);
}

bool ADCOverUART::setSamplingInterval(uint8_t channel, uint16_t interval) {
  return this->send(SET_SAMPLING_INTERVAL, channel, interval);
}

bool ADCOverUART::setSamplingTime(uint8_t channel, uint16_t time) {
  return this->send(SET_SAMPLING_TIME, channel, time);
}

bool ADCOverUART::setDeltaTime(uint8_t channel, uint16_t time) {
  return this->send(SET_DELTA_TIME, channel, time);
}

bool ADCOverUART::read(uint8_t channel) {
  this->flush();
  return this->send(MEASURE_RUNNING, channel) && this->readData(channel);
}

uint16_t ADCOverUART::getReading(uint8_t channel) {
  return this->results[channel];
}

float ADCOverUART::getVoltage(uint8_t channel) {
  return this->results[channel] * MAX_READING_VALUE / MAX_RAW_READING_VALUE;
}

bool ADCOverUART::send(uint8_t command) {
  this->serial->write(HEADER);
  this->serial->write(command);
  return this->ack();
}

bool ADCOverUART::send(uint8_t command, uint8_t channel) {
  this->serial->write(HEADER);
  this->serial->write(command);
  this->serial->write(channel);
  return this->ack();
}

bool ADCOverUART::send(uint8_t command, uint8_t channel, uint16_t value) {
  uint8_t low = value & 0xFF;
  uint8_t high = value >> 8;

  this->serial->write(HEADER);
  this->serial->write(command);
  this->serial->write(channel);
  this->serial->write(low);
  this->serial->write(high);
  return this->ack();
}

bool ADCOverUART::await(uint16_t time, uint8_t numBytes) {
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

bool ADCOverUART::ack() {
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

bool ADCOverUART::readData(uint8_t channel) {
  if(!this->await(100, 2)) {
    return false;
  }

  uint8_t low = this->serial->read();
  uint8_t high = this->serial->read();
  uint16_t raw = (uint16_t)(high) << 8 | low;

  this->results[channel] = raw;

  return true;
}

void ADCOverUART::flush() {
  // NOTE In case of a previous read error this will set the stage for the next reading.
  while(this->serial->available() > 0) {
    this->serial->read();
  }
}
