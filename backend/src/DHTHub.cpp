#include "DHTHub.hpp"

DHTHub::DHTHub(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(Reading<float>("humidity", "DHT", "humidity", new WindowedValue<float, SAMPLE_BACKLOG>("%", 0, 100))),
    temperature(Reading<float>("temperature", "DHT", "temperature", new WindowedValue<float, SAMPLE_BACKLOG>("°C", 0, 50)))
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
