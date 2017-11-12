#include "HTUHub.hpp"

HTUHub::HTUHub(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(HTU21D()),
    humidity(Sensor("HTU", "humidity", "humidity", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("HTU", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void HTUHub::begin() {
  Wire.begin(this->sda, this->scl);
  this->sensor.begin(Wire);
}

void HTUHub::update() {
  float hum = this->sensor.readHumidity();
  if (hum == ERROR_I2C_TIMEOUT || hum == ERROR_BAD_CRC) {
    this->humidity.add(NAN);
  } else {
    this->humidity.add(hum);
  }

  float temp = this->sensor.readTemperature();
  if (temp == ERROR_I2C_TIMEOUT || temp == ERROR_BAD_CRC) {
    this->temperature.add(NAN);
  } else {
    this->temperature.add(temp);
  }
}

void HTUHub::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}
