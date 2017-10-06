#include "DallasTempHub.hpp"

DallasTempHub::DallasTempHub(uint8_t pin, uint8_t resolution):
    oneWire(OneWire(pin)),
    sensors(DallasTemperature(&this->oneWire))
{
  this->sensors.setResolution(resolution);
  this->sensors.setWaitForConversion(false);
}

DallasTempHub::~DallasTempHub() {
  if(this->temperatures != nullptr) {
    delete this->temperatures;
  }
}

void DallasTempHub::begin() {
  this->sensors.begin();
  this->nSensors = this->sensors.getDeviceCount();
  this->temperatures = new WindowedReading<float, SAMPLE_BACKLOG>[this->nSensors];
  this->sensors.requestTemperatures();
}

void DallasTempHub::update() {
  for(uint8_t i = 0; i < this->nSensors; ++i) {
    float temp = this->sensors.getTempCByIndex(i);

    if(!isnan(temp) && temp != DEVICE_DISCONNECTED_C && temp != DEVICE_DISCONNECTED_RAW) {
      this->temperatures[i].add(temp);
      // TODO Handle errors.
      // this->nMeasurements++;
      // this->sStatus = "ok";
    } else {
      // this->sStatus = "error";
      // this->nErrors++;
    }
  }

  this->sensors.requestTemperatures();
}

void DallasTempHub::connect(Device *d) {
  for(uint8_t i = 0; i < this->nSensors; ++i) {
    d->attach(new Sensor("DallasTemperature", "temperature", "temperature" + String(i), this->temperatures[i]));
  }
}
