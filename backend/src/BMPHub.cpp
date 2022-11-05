#include "BMPHub.hpp"

BMPHub::BMPHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_BMP280(i2c)),
    pressure(Sensor<float>("pressure", "BMP", "pressure", new WindowedReading<float, SAMPLE_BACKLOG>("Pa", 30000, 110000))),
    temperature(Sensor<float>("temperature", "BMP", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>("°C", -40, 85))),
    altitude(Sensor<float>("altitude", "BMP", "altitude", new WindowedReading<float, SAMPLE_BACKLOG>("m", -500, 9000)))
{}

void BMPHub::begin(System &system) {
  this->sensor.begin(this->address);

  system.device().attach(this);

  system.scheduler().spawn("sample BMP", 115,[&](Task *t) {
    system.log().debug("Sampling BMP hub.");
    this->update();
    t->sleep(BMP_SAMPLE_INTERVAL);
  });
}

void BMPHub::update() {
  this->pressure.add(this->sensor.readPressure());
  this->temperature.add(this->sensor.readTemperature());
  this->altitude.add(this->sensor.readAltitude());
}

void BMPHub::connect(Device *d) const {
  d->attach(&this->pressure);
  d->attach(&this->temperature);
  d->attach(&this->altitude);
}
