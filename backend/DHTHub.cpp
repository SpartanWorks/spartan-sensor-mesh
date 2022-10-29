#include "DHTHub.hpp"

DHTHub::DHTHub(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(Sensor<float>("humidity", "DHT", "humidity", new WindowedReading<float, SAMPLE_BACKLOG>("%", 20, 90))),
    temperature(Sensor<float>("temperature", "DHT", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>("°C", 0, 50)))
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
