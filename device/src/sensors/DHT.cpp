#include "DHT.hpp"

ssn::DHT::DHT(uint8_t pin, uint8_t model, uint16_t interval):
    sensor(::DHT(pin, model)),
    sampleInterval(interval),
    humidity(nullptr),
    temperature(nullptr)
{}

ssn::DHT::~DHT() {
  if (this->humidity != nullptr) delete this->humidity;
  if (this->temperature != nullptr) delete this->temperature;
}

ssn::DHT* ssn::DHT::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t pin = (int) conn["pin"];
  JSONVar readings = config["readings"];

  if (conn == undefined || readings == undefined || (bus != "dht11" && bus != "dht22")) {
    return nullptr;
  }

  uint8_t model = DHT11;
  float tMin = 0;
  float tMax = 50;

  if (bus == "dht22") {
    model = DHT22;
    tMin = -40;
    tMax = 80;
  }

  ssn::DHT *dht = new ssn::DHT(pin, model, interval);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "humidity") {
      dht->humidity = new Reading<float>(name, "DHT", type, new WindowedValue<float>(window, "%", 0, 100), cfg);
    } else if (type == "temperature") {
      dht->temperature = new Reading<float>(name, "DHT", type, new WindowedValue<float>(window, "°C", tMin, tMax), cfg);
    }
  }

  return dht;
}

void ssn::DHT::begin(System &system) {
  this->sensor.begin();

  system.device().attach(this);

  system.scheduler().spawn("sample DHT", 115,[&](Task *t) {
    system.log().debug("Sampling DHT sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void ssn::DHT::update() {
  if (this->humidity != nullptr) {
    this->humidity->add(this->sensor.readHumidity());
  }
  if (this->temperature != nullptr) {
    this->temperature->add(this->sensor.readTemperature());
  }
}

void ssn::DHT::connect(Device *d) const {
  if (this->humidity != nullptr) {
    d->attach(this->humidity);
  }
  if (this->temperature != nullptr) {
    d->attach(this->temperature);
  }
}
