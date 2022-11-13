#include "DallasTemp.hpp"

DallasTemp::DallasTemp(uint8_t pin, uint8_t resolution, uint16_t interval):
    oneWire(OneWire(pin)),
    sensors(DallasTemperature(&this->oneWire)),
    sampleInterval(interval)
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

DallasTemp* DallasTemp::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t pin = (int) conn["pin"];
  uint16_t resolution = (int) config["resolution"];

  JSONVar reading = config["readings"][0];
  String type = (const char*) reading["type"];
  String name = (const char*) reading["name"];
  uint16_t window = (int) reading["averaging"];
  JSONVar cfg = reading["widget"];

  if(conn == undefined || reading == undefined || bus != "dallas-1-wire" || type != "temperature" ) {
    return nullptr;
  }

  DallasTemp *dallas = new DallasTemp(pin, resolution, interval);
  dallas->sensors.begin();

  dallas->nReadings = dallas->sensors.getDeviceCount();
  for(uint8_t i = 0; i < dallas->nReadings; ++i) {
    String readingName = name;
    readingName.replace("#", String(i));
    Reading<float> *s = new Reading<float>(readingName, "DallasTemp", type, new WindowedValue<float>(window, "°C", -55, 125), cfg);
    dallas->temperatures = new List<Temp>(Temp(i, s), dallas->temperatures);
  }

  return dallas;
}

void DallasTemp::begin(System &system) {
  // NOTE The sensor has already been started technically, so we restart it here.
  this->sensors.begin();
  this->sensors.requestTemperatures();

  system.device().attach(this);

  system.scheduler().spawn("sample Dallas", 115,[&](Task *t) {
    system.log().debug("Sampling Dallas sensor.");
    this->update();
    t->sleep(this->sampleInterval);
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
