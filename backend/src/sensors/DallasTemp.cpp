#include "DallasTemp.hpp"

DallasTemp::DallasTemp(uint8_t pin, uint8_t resolution):
    oneWire(OneWire(pin)),
    sensors(DallasTemperature(&this->oneWire))
{
  this->sensors.setResolution(resolution);
  this->sensors.setWaitForConversion(false);
}

DallasTemp::~DallasTemp() {
  if (this->temperatures != nullptr) {
    foreach<Temp>(this->temperatures, [](Temp t) {
      delete t.reading;
    });
    delete this->temperatures;
  }
}

void DallasTemp::begin(System &system) {
  this->sensors.begin();
  this->nReadings = this->sensors.getDeviceCount();
  for(uint8_t i = 0; i < this->nReadings; ++i) {
    Value<float> *r = new WindowedValue<float, SAMPLE_BACKLOG>("°C", -55, 125);
    Reading<float> *s = new Reading<float>("temperature" + String(i), "DallasTemperature", "temperature", r);
    this->temperatures = new List<Temp>(Temp(i, s), this->temperatures);
  }
  this->sensors.requestTemperatures();

  system.device().attach(this);

  system.scheduler().spawn("sample Dallas", 115,[&](Task *t) {
    system.log().debug("Sampling Dallas sensor.");
    this->update();
    t->sleep(DALLAS_SAMPLE_INTERVAL);
  });
}

void DallasTemp::update() {
  foreach<Temp>(this->temperatures, [this](Temp t) {
    float temp = this->sensors.getTempCByIndex(t.index);
    if(temp != DEVICE_DISCONNECTED_C && temp != DEVICE_DISCONNECTED_RAW) {
      t.reading->add(temp);
    } else {
      t.reading->setError(String("Could not read sensor. Response: ") + String(temp));
    }
  });
  this->sensors.requestTemperatures();
}

void DallasTemp::connect(Device *d) const {
  foreach<Temp>(this->temperatures, [d](Temp t) {
    d->attach(t.reading);
  });
}
