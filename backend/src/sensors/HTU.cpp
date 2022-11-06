#include "HTU.hpp"

HTU::HTU(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(HTU21D()),
    humidity(Reading<float>("humidity", "HTU", "humidity", new WindowedValue<float, SAMPLE_BACKLOG>("%", 0, 100))),
    temperature(Reading<float>("temperature", "HTU", "temperature", new WindowedValue<float, SAMPLE_BACKLOG>("°C", -40, 125))),
    toCompensate(nullptr)
{}

HTU::~HTU() {
  if (this->toCompensate != nullptr) {
    delete this->toCompensate;
  }
}

void HTU::begin(System &system) {
  this->sensor.begin(*(this->i2c));

  system.device().attach(this);

  system.scheduler().spawn("sample HTU", 115,[&](Task *t) {
    system.log().debug("Sampling HTU sensor.");
    this->update();
    t->sleep(HTU_SAMPLE_INTERVAL);
  });
}

void HTU::update() {
  float hum = this->sensor.readHumidity();
  if (hum == ERROR_I2C_TIMEOUT || hum == ERROR_BAD_CRC) {
    this->humidity.setError(String("Could not read sensor. Response: ") + String(hum));
  } else {
    this->humidity.add(hum);
  }

  float temp = this->sensor.readTemperature();
  if (temp == ERROR_I2C_TIMEOUT || temp == ERROR_BAD_CRC) {
    this->temperature.setError(String("Could not read sensor. Response: ") + String(temp));
  } else {
    this->temperature.add(temp);
  }

  if (temp != ERROR_I2C_TIMEOUT && temp != ERROR_BAD_CRC && hum != ERROR_I2C_TIMEOUT && hum != ERROR_BAD_CRC) {
    foreach<Sensor*>(this->toCompensate, [=](Sensor *s) {
      s->setCompensationParameters(temp, hum);
    });
  }
}

void HTU::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}

void HTU::compensate(Sensor *s) {
  this->toCompensate = new List<Sensor*>(s, this->toCompensate);
}
