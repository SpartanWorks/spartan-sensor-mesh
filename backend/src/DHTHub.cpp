#include "DHTHub.hpp"

DHTHub::DHTHub(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(Sensor<float>("humidity", "DHT", "humidity", new WindowedReading<float, SAMPLE_BACKLOG>("%", 20, 90))),
    temperature(Sensor<float>("temperature", "DHT", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>("°C", 0, 50)))
{}

void DHTHub::begin(System &system) {
  this->sensor.begin();

  system.device().attach(this);

  system.scheduler().spawn("sample DHT", 115,[&](Task *t) {
    system.log().debug("Sampling DHT hub.");
    this->update();
    t->sleep(DHT_SAMPLE_INTERVAL);
  });
}

void DHTHub::update() {
  this->humidity.add(this->sensor.readHumidity());
  this->temperature.add(this->sensor.readTemperature());
}

void DHTHub::connect(Device *d) const {
  d->attach(&this->humidity);
  d->attach(&this->temperature);
}
