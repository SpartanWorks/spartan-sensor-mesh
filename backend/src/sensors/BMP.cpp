#include "BMP.hpp"

BMP::BMP(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_BMP280(i2c)),
    pressure(Reading<float>("pressure", "BMP", "pressure", new WindowedValue<float, SAMPLE_BACKLOG>("Pa", 30000, 110000))),
    temperature(Reading<float>("temperature", "BMP", "temperature", new WindowedValue<float, SAMPLE_BACKLOG>("°C", -40, 85))),
    altitude(Reading<float>("altitude", "BMP", "altitude", new WindowedValue<float, SAMPLE_BACKLOG>("m", -500, 9000)))
{}

void BMP::begin(System &system) {
  this->sensor.begin(this->address);

  system.device().attach(this);

  system.scheduler().spawn("sample BMP", 115,[&](Task *t) {
    system.log().debug("Sampling BMP sensor.");
    this->update();
    t->sleep(BMP_SAMPLE_INTERVAL);
  });
}

void BMP::update() {
  this->pressure.add(this->sensor.readPressure());
  this->temperature.add(this->sensor.readTemperature());
  this->altitude.add(this->sensor.readAltitude());
}

void BMP::connect(Device *d) const {
  d->attach(&this->pressure);
  d->attach(&this->temperature);
  d->attach(&this->altitude);
}
