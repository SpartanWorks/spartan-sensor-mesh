#include "HTUHub.hpp"

HTUHub::HTUHub(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(HTU21D()),
    humidity(Sensor<float>("humidity", "HTU", "humidity", "%", 0, 100, new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor<float>("temperature", "HTU", "temperature", "°C", -40, 125, new WindowedReading<float, SAMPLE_BACKLOG>())),
    toCompensate(nullptr)
{}

HTUHub::~HTUHub() {
  if (this->toCompensate != nullptr) {
    delete this->toCompensate;
  }
}

void HTUHub::begin() {
  this->sensor.begin(*(this->i2c));
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
