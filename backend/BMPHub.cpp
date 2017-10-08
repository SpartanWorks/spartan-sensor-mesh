#include "BMPHub.hpp"

BMPHub::BMPHub(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(Adafruit_BMP280()),
    pressure(Sensor("BMP", "pressure", "pressure", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("BMP", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void BMPHub::begin() {
  Wire.begin(this->sda, this->scl);
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
