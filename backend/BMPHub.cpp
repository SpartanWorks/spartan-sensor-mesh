#include "BMPHub.hpp"

BMPHub::BMPHub(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(Adafruit_BMP280())
{}

void BMPHub::begin() {
  Wire.begin(this->sda, this->scl);
  this->sensor.begin(this->address);
}

void BMPHub::update() {
  float press = this->sensor.readPressure();
  float temp = this->sensor.readTemperature();

  if(!isnan(press) && !isnan(temp)) {
    this->pressure.add(press);
    this->temperature.add(temp);
    // TODO Handle errors.
    // this->nMeasurements++;
    // this->sStatus = "ok";
  } else {
    // this->nErrors++;
    // this->sStatus = "error";
  }
}

void BMPHub::connect(Device *d) {
  d->attach(new Sensor("BMP", "pressure", "pressure", this->pressure));
  d->attach(new Sensor("BMP", "temperature", "temperature", this->temperature));
}
