#include "HTU.hpp"

HTU::HTU(TwoWire *i2c, uint8_t addr, uint16_t interval):
    i2c(i2c),
    address(addr),
    sensor(Adafruit_HTU21DF()),
    sampleInterval(interval),
    humidity(nullptr),
    temperature(nullptr),
    toCompensate(nullptr)
{}

HTU::~HTU() {
  if (this->toCompensate != nullptr) delete this->toCompensate;
  if (this->humidity != nullptr) delete this->humidity;
  if (this->temperature != nullptr) delete this->temperature;
}

HTU* HTU::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t address = (int) conn["address"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "hardware-i2c") {
    return nullptr;
  }

  HTU *htu = new HTU(&Wire, address, interval);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "humidity") {
      htu->humidity = new Reading<float>(name, "HTU", type, new WindowedValue<float>(window, "%", 0, 100), cfg);
    } else if (type == "temperature") {
      htu->temperature = new Reading<float>(name, "HTU", type, new WindowedValue<float>(window, "°C", -40, 125), cfg);
    }
  }

  return htu;
}

void HTU::begin(System &system) {
  this->sensor.begin(this->i2c);

  system.device().attach(this);

  system.scheduler().spawn("sample HTU", 115,[&](Task *t) {
    system.log().debug("Sampling HTU sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void HTU::update() {
  float hum = 0;

  if (this->humidity != nullptr) {
    hum = this->sensor.readHumidity();
    if (hum == NAN) {
      this->humidity->setError(String("Could not read sensor. Response: ") + String(hum));
    } else {
      this->humidity->add(hum);
    }
  }

  float temp = 0;

  if (this->temperature != nullptr) {
    temp = this->sensor.readTemperature();
    if (temp == NAN) {
      this->temperature->setError(String("Could not read sensor. Response: ") + String(temp));
    } else {
      this->temperature->add(temp);
    }
  }

  if (this->temperature != nullptr
      && temp != NAN
      && this->humidity != nullptr
      && hum != NAN) {
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
