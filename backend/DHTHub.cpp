#include "DHTHub.hpp"

DHTHub::DHTHub(uint8_t pin, uint8_t model):
    sensor(DHT(pin, model)),
    humidity(WindowedReading<float, SAMPLE_BACKLOG>()),
    temperature(WindowedReading<float, SAMPLE_BACKLOG>())
{}

void DHTHub::begin() {
  this->sensor.begin();
}

void DHTHub::update() {
  float hum = this->sensor.readHumidity();
  float temp = this->sensor.readTemperature();

  if(!isnan(hum) && !isnan(temp)) {
    this->humidity.add(hum);
    this->temperature.add(temp);
    // TODO Handle errors.
    // this->nMeasurements++;
    // this->sStatus = "ok";
  } else {
    // this->nErrors++;
    // this->sStatus = "error";
  }
}

void DHTHub::connect(Device *d) {
  d->attach(new Sensor("DHT", "humidity", "pressure", &this->humidity));
  d->attach(new Sensor("DHT", "temperature", "temperature", &this->temperature));
}
