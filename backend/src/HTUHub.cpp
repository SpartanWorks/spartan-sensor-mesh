#include "HTUHub.hpp"

HTUHub::HTUHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(HTU21D()),
    humidity(Sensor<float>("humidity", "HTU", "humidity", new WindowedValue<float, SAMPLE_BACKLOG>("%", 0, 100))),
    temperature(Sensor<float>("temperature", "HTU", "temperature", new WindowedValue<float, SAMPLE_BACKLOG>("°C", -40, 125))),
    toCompensate(nullptr)
{}

HTUHub::~HTUHub() {
  if (this->toCompensate != nullptr) {
    delete this->toCompensate;
  }
}

void HTUHub::begin(System &system) {
  this->sensor.begin(*(this->i2c));

  system.device().attach(this);

  system.scheduler().spawn("sample HTU", 115,[&](Task *t) {
    system.log().debug("Sampling HTU hub.");
    this->update();
    t->sleep(HTU_SAMPLE_INTERVAL);
  });
}

void HTUHub::update() {
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
    foreach<SensorHub*>(this->toCompensate, [=](SensorHub *s) {
      s->setCompensationParameters(temp, hum);
    });
  }
}

void HTUHub::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}

void HTUHub::compensate(SensorHub *s) {
  this->toCompensate = new List<SensorHub*>(s, this->toCompensate);
}
