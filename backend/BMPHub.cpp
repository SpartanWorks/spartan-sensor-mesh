#include "BMPHub.hpp"

BMPHub::BMPHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_BMP280(i2c)),
    pressure(Sensor("BMP", "pressure", "pressure", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("BMP", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void BMPHub::begin() {
  this->sensor.begin(this->address);
}

void BMPHub::update() {
  this->pressure.add(this->sensor.readPressure());
  this->temperature.add(this->sensor.readTemperature());
}

void BMPHub::connect(Device *d) const {
  d->attach(&this->pressure);
  d->attach(&this->temperature);
}
