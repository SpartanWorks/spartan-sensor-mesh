#include "HTU.hpp"

HTU::HTU(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(HTU21D()),
    humidity(),
    temperature(nullptr),
    toCompensate(nullptr)
{}

HTU::~HTU() {
  if (this->toCompensate != nullptr) delete this->toCompensate;
  if (this->humidity != nullptr) delete this->humidity;
  if (this->temperature != nullptr) delete this->temperature;
}

HTU* HTU::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t address = (int) conn["address"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-i2c") {
    return nullptr;
  }

  HTU *htu = new HTU(&Wire, address);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "humidity") {
      htu->humidity = new Reading<float>(name, "HTU", type, new WindowedValue<float, SAMPLE_BACKLOG>("%", 0, 100), cfg);
    } else if (type == "temperature") {
      htu->temperature = new Reading<float>(name, "HTU", type, new WindowedValue<float, SAMPLE_BACKLOG>("°C", -40, 125), cfg);
    }
  }

  return htu;
}

void HTU::begin(System &system) {
  this->sensor.begin(*(this->i2c));

  system.device().attach(this);

  system.scheduler().spawn("sample HTU", 115,[&](Task *t) {
    system.log().debug("Sampling HTU sensor.");
    this->update();
    t->sleep(HTU_SAMPLE_INTERVAL);
  });
}

void HTU::update() {
  float hum = 0;

  if (this->humidity != nullptr) {
    hum = this->sensor.readHumidity();
    if (hum == ERROR_I2C_TIMEOUT || hum == ERROR_BAD_CRC) {
      this->humidity->setError(String("Could not read sensor. Response: ") + String(hum));
    } else {
      this->humidity->add(hum);
    }
  }

  float temp = 0;

  if (this->temperature != nullptr) {
    temp = this->sensor.readTemperature();
    if (temp == ERROR_I2C_TIMEOUT || temp == ERROR_BAD_CRC) {
      this->temperature->setError(String("Could not read sensor. Response: ") + String(temp));
    } else {
      this->temperature->add(temp);
    }
  }

  if (this->temperature != nullptr
      && temp != ERROR_I2C_TIMEOUT
      && temp != ERROR_BAD_CRC
      && this->humidity != nullptr
      && hum != ERROR_I2C_TIMEOUT
      && hum != ERROR_BAD_CRC) {
    foreach<Sensor*>(this->toCompensate, [=](Sensor *s) {
      s->setCompensationParameters(temp, hum);
    });
  }
}

void HTU::connect(Device *d) const {
  if (this->humidity != nullptr) {
    d->attach(this->humidity);
  }
  if (this->temperature != nullptr) {
    d->attach(this->temperature);
  }
}

void HTU::compensate(Sensor *s) {
  this->toCompensate = new List<Sensor*>(s, this->toCompensate);
}
