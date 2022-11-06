#include "DHT.hpp"

ssn::DHT::DHT(uint8_t pin, uint8_t model):
    sensor(::DHT(pin, model)),
    humidity(nullptr),
    temperature(nullptr)
{}

ssn::DHT::~DHT() {
  if (this->humidity != nullptr) delete this->humidity;
  if (this->temperature != nullptr) delete this->temperature;
}

ssn::DHT* ssn::DHT::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t pin = (int) conn["pin"];
  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || (bus != "dht11" && bus != "dht22")) {
    return nullptr;
  }

  ssn::DHT *dht = new ssn::DHT(pin, (bus == "dht22") ? DHT22 : DHT11);

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    JSONVar cfg = readings[i]["widget"];

    if (type == "humidity") {
      dht->humidity = new Reading<float>(name, "DHT", type, new WindowedValue<float, SAMPLE_BACKLOG>("%", 0, 100), cfg);
    } else if (type == "temperature") {
      dht->temperature = new Reading<float>(name, "DHT", type, new WindowedValue<float, SAMPLE_BACKLOG>("°C", 0, 50), cfg);
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
    t->sleep(DHT_SAMPLE_INTERVAL);
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
