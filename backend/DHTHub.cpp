#include "DHTHub.hpp"

DHTHub::DHTHub(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(Sensor<float>("humidity", "DHT", "humidity", "%", 20, 90, new WindowedReading<float, SAMPLE_BACKLOG>())),
    temperature(Sensor<float>("temperature", "DHT", "temperature", "°C", 0, 50, new WindowedReading<float, SAMPLE_BACKLOG>()))
{}

void DHTHub::begin() {
  this->sensor.begin();
}

void DHTHub::update() {
  this->humidity.add(this->sensor.readHumidity());
  this->temperature.add(this->sensor.readTemperature());
}

void DHTHub::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}
