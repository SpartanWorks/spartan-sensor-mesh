#include "BMP.hpp"

BMP::BMP(TwoWire *i2c, uint8_t addr, uint16_t interval):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_BMP280(i2c)),
    sampleInterval(interval),
    pressure(nullptr),
    temperature(nullptr),
    altitude(nullptr)
{}

BMP::~BMP() {
  if (this->pressure) delete this->pressure;
  if (this->temperature) delete this->temperature;
  if (this->altitude) delete this->altitude;
}

BMP* BMP::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t address = (int) conn["address"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-i2c") {
    return nullptr;
  }

  BMP *bmp = new BMP(&Wire, address, interval);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "pressure") {
      bmp->pressure = new Reading<float>(name, "BMP", type, new WindowedValue<float>(window, "hPa", 300, 1100), cfg);
    } else if (type == "temperature") {
      bmp->temperature = new Reading<float>(name, "BMP", type, new WindowedValue<float>(window, "°C", -40, 85), cfg);
    } else if (type == "altitude") {
      bmp->altitude = new Reading<float>(name, "BMP", type, new WindowedValue<float>(window, "m", -500, 9000), cfg);
    }
  }

  return bmp;
}

void BMP::begin(System &system) {
  this->sensor.begin(this->address);

  system.device().attach(this);

  system.scheduler().spawn("sample BMP", 115,[&](Task *t) {
    system.log().debug("Sampling BMP sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void BMP::update() {
  if (this->pressure != nullptr) {
    this->pressure->add(this->sensor.readPressure() / 100.0);
  }

  if (this->temperature != nullptr) {
    this->temperature->add(this->sensor.readTemperature());
  }

  if (this->altitude != nullptr) {
    this->altitude->add(this->sensor.readAltitude());
  }
}

void BMP::connect(Device *d) const {
  if (this->pressure != nullptr) {
    d->attach(this->pressure);
  }
  if (this->temperature != nullptr) {
    d->attach(this->temperature);
  }

  if (this->altitude != nullptr) {
    d->attach(this->altitude);
  }
}
