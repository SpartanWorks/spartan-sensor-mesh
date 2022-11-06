#include "MHZ.hpp"

MHZ::MHZ(uint8_t rx, uint8_t tx):
    sensor(MHZ19()),
    co2(Reading<float>("CO2", "MHZ", "co2", new WindowedValue<float, SAMPLE_BACKLOG>("ppm", 0, 10000))),
    temperature(Reading<float>("temperature", "MHZ", "temperature", new WindowedValue<float, SAMPLE_BACKLOG>("°C", 0, 100)))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void MHZ::initSensor() {
  this->sensor.begin(*(this->serial));
  this->sensor.autoCalibration(false);
  this->sensor.calibrate();
}

void MHZ::begin(System &system) {
  this->serial->begin(MHZ_BAUDRATE);
  this->initSensor();

  system.device().attach(this);

  system.scheduler().spawn("sample MHZ", 115,[&](Task *t) {
    system.log().debug("Sampling MHZ sensor.");
    this->update();
    t->sleep(MHZ_SAMPLE_INTERVAL);
  });

  system.scheduler().spawn("reset MHZ", 125,[&](Task *t) {
    static boolean mhzWarmup = true;

    if (mhzWarmup) {
      mhzWarmup = false;
      t->sleep(MHZ_WARMUP_TIMEOUT);
    } else {
      system.log().info("Resetting MHZ sensor after a warmup.");
      this->reset();
      t->kill();
    }
  });
}

void MHZ::update() {
  float co2 = this->sensor.getCO2();
  if(this->sensor.errorCode == RESULT_OK) {
    this->co2.add(co2);
  } else {
    this->co2.setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
  }

#ifdef MHZ_TEMP_SENSOR
  float temp = this->sensor.getTemperature();
  if(this->sensor.errorCode == RESULT_OK) {
    this->temperature.add(temp);
  } else {
    this->co2.setError(String("Could not read sensor. Response: ") + String(this->sensor.errorCode));
  }
#endif
}

void MHZ::connect(Device *d) const {
  d->attach(&this->co2);
#ifdef MHZ_TEMP_SENSOR
  d->attach(&this->temperature);
#endif
}

void MHZ::reset() {
  this->initSensor();
}
