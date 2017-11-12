#include "HTUHub.hpp"

HTUHub::HTUHub(uint8_t da, uint8_t cl, uint8_t addr):
    sda(da),
    scl(cl),
    address(addr),
    sensor(Adafruit_HTU21DF()),
    humidity(Sensor("HTU", "humidity", "humidity", new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor("HTU", "temperature", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void HTUHub::begin() {
  Wire.begin(this->sda, this->scl);
  this->sensor.begin();
}

void HTUHub::update() {
  this->humidity.add(this->sensor.readHumidity());
  this->temperature.add(this->sensor.readTemperature());
}

void HTUHub::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}
