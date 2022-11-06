#include "SGP.hpp"

SGP::SGP(TwoWire *i2c, uint8_t addr):
    i2c(i2c),
    address(addr),
    sensor(SGP30(i2c)),
    voc(Reading<float>("Total VOC", "SGP", "voc", new WindowedValue<float, SAMPLE_BACKLOG>("ppb", 0, 60000))),
    co2(Reading<float>("CO2", "SGP", "co2", new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 57330))),
    h2(Reading<float>("H2", "SGP", "voc", new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 1000))),
    ethanol(Reading<float>("Ethanol", "SGP", "voc", new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 1000)))
{}

void SGP::begin(System &system) {
  this->sensor.begin(); // FIXME Use this->address during init.

  this->co2.setStatus("init");
  this->voc.setStatus("init");
  this->ethanol.setStatus("init");
  this->h2.setStatus("init");

  uint32_t t0 = millis();
  uint32_t t1 = t0;
  while(!this->sensor.isConnected(), (t1 - t0) < SGP_INIT_TIME) {
    delay(10);
    t1 = millis();
  }

  if(!this->sensor.isConnected()) {
    String error = String("Sensor initialization took more than ") + String(SGP_INIT_TIME) + "ms.";
    this->co2.setError(error);
    this->voc.setError(error);
    this->h2.setError(error);
    this->ethanol.setError(error);
  } else {
    this->sensor.measureTest();
    this->sensor.GenericReset();
  }

  system.device().attach(this);

  system.scheduler().spawn("sample SGP", 115,[&](Task *t) {
    system.log().debug("Sampling SGP sensor.");
    this->update();
    t->sleep(SGP_SAMPLE_INTERVAL);
  });

  system.scheduler().spawn("reset SGP", 125,[&](Task *t) {
    static boolean sgpWarmup = true;

    if (sgpWarmup) {
      sgpWarmup = false;
      t->sleep(SGP_WARMUP_TIMEOUT);
    } else {
      system.log().info("Resetting SGP sensor after a warmup.");
      this->sensor.GenericReset();
      t->kill();
    }
  });
}

void SGP::update() {
  if(!this->sensor.isConnected()) {
    String error = "Sensor is not available.";
    this->co2.setError(error);
    this->voc.setError(error);
    this->h2.setError(error);
    this->ethanol.setError(error);
    return;
  }

  bool result = this->sensor.measure(true);

  if(result) {
    this->co2.add(this->sensor.getCO2());
    this->voc.add(this->sensor.getTVOC());
    this->h2.add(this->sensor.getH2());
    this->ethanol.add(this->sensor.getEthanol());
  } else {
    String error = String("Could not read sensor:" + String(this->sensor.lastError()));
    this->co2.setError(error);
    this->voc.setError(error);
    this->h2.setError(error);
    this->ethanol.setError(error);
  }
}

void SGP::connect(Device *d) const {
  d->attach(&this->co2);
  d->attach(&this->voc);
  d->attach(&this->h2);
  d->attach(&this->ethanol);
}

void SGP::setCompensationParameters(float temperature, float humidity) {
  this->sensor.setRelHumidity(temperature, humidity);
}
