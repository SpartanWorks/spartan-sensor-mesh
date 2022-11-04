#include "MHZHub.hpp"

MHZHub::MHZHub(uint8_t rx, uint8_t tx):
    sensor(MHZ19()),
    co2(Sensor<float>("CO2", "MHZ", "co2", new WindowedReading<float, SAMPLE_BACKLOG>("ppm", 0, 10000))),
    temperature(Sensor<float>("temperature", "MHZ", "temperature", new WindowedReading<float, SAMPLE_BACKLOG>("°C", 0, 100)))
{
  this->serial = new SoftwareSerial(rx, tx);
}

void MHZHub::initSensor() {
  this->sensor.begin(*(this->serial));
  this->sensor.autoCalibration(false);
  this->sensor.calibrate();
}

void MHZHub::begin(System &system) {
  this->serial->begin(MHZ_BAUDRATE);
  this->initSensor();

  system.device().attach(this);

  system.scheduler().spawn("sample MHZ", 115,[&](Task *t) {
    system.log().debug("Sampling MHZ hub.");
    this->update();
    t->sleep(MHZ_SAMPLE_INTERVAL);
  });

  system.scheduler().spawn("reset MHZ", 125,[&](Task *t) {
    static boolean mhzWarmup = true;

    if (mhzWarmup) {
      mhzWarmup = false;
      t->sleep(MHZ_WARMUP_TIMEOUT);
    } else {
      system.log().info("Resetting MHZ hub after a warmup.");
      this->reset();
      t->kill();
    }
  });
}

void MHZHub::update() {
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

void MHZHub::connect(Device *d) const {
  d->attach(&this->co2);
#ifdef MHZ_TEMP_SENSOR
  d->attach(&this->temperature);
#endif
}

void MHZHub::reset() {
  this->initSensor();
}
