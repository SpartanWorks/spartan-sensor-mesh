#include "DallasTempHub.hpp"

DallasTempHub::DallasTempHub(uint8_t pin, uint8_t resolution):
    oneWire(OneWire(pin)),
    sensors(DallasTemperature(&this->oneWire))
{
  this->sensors.setResolution(resolution);
  this->sensors.setWaitForConversion(false);
}

DallasTempHub::~DallasTempHub() {
  if (this->temperatures != nullptr) {
    foreach<Temp>(this->temperatures, [](Temp t) {
      delete t.sensor;
    });
    delete this->temperatures;
  }
}

void DallasTempHub::begin() {
  this->sensors.begin();
  this->nSensors = this->sensors.getDeviceCount();
  for(uint8_t i = 0; i < this->nSensors; ++i) {
    Reading<float> *r = new WindowedReading<float, SAMPLE_BACKLOG>();
    Sensor *s = new Sensor("DallasTemperature", "temperature", "temperature" + String(i), r);
    this->temperatures = new List<Temp>(Temp(i, s), this->temperatures);
  }
  this->sensors.requestTemperatures();
}

void DallasTempHub::update() {
  foreach<Temp>(this->temperatures, [this](Temp t) {
    float temp = this->sensors.getTempCByIndex(t.index);
    if(temp != DEVICE_DISCONNECTED_C && temp != DEVICE_DISCONNECTED_RAW) {
      t.sensor->add(temp);
    } else {
      t.sensor->add(NAN);
    }
  });
  this->sensors.requestTemperatures();
}

void DallasTempHub::connect(Device *d) {
  foreach<Temp>(this->temperatures, [d](Temp t) {
    d->attach(t.sensor);
  });
}
