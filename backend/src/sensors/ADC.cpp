#include "ADC.hpp"

ADCChannel::ADCChannel(JSONVar &config, Reading<float> *s, Reading<float> *r):
    scaled(s),
    raw(r)
{
  this->index = (int) config["number"];
  this->samplingInterval = (int) config["samplingInterval"];
  this->samplingTime = (int) config["samplingTime"];
  this->deltaTime = (int) config["deltaTime"];
  this->min = (double) config["min"];
  this->offset = (double) config["offset"];
  this->factor = (double) config["factor"];
  this->baseline = 0;
}

void ADCChannel::add(uint16_t raw, float volts) {
  if (this->raw != nullptr) this->raw->add(raw);

  if (this->scaled != nullptr) {
    float result = this->factor * volts + this->offset + this->baseline;

    if (result < this->min) {
      this->baseline += this->min - result;
    }

    this->scaled->add(result);
  }
}

void ADCChannel::setError(String error) {
  if (this->raw != nullptr) this->raw->setError(error);
}

ADC::ADC(uint8_t rx, uint8_t tx, uint8_t numChannels, List<ADCChannel> *channels):
    sensor(ADCOverUART(rx, tx, numChannels)),
    channels(channels)
{}

ADC::~ADC() {
  if (this->channels != nullptr) {
    foreach<ADCChannel>(this->channels, [](ADCChannel c) {
      if (c.scaled != nullptr) delete c.scaled;
      if (c.raw != nullptr) delete c.raw;
    });
    delete this->channels;
  }
}

ADC* ADC::create(JSONVar &config) {
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];
  uint16_t rx = (int) conn["rx"];
  uint16_t tx = (int) conn["tx"];

  JSONVar readings = config["readings"];

  if(conn == undefined || readings == undefined || bus != "software-uart") {
    return nullptr;
  }

  uint8_t numChannels = 0;

  List<ADCChannel> *channels = nullptr;

  for(uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    String unit = (const char*) readings[i]["unit"];

    JSONVar channel = readings[i]["channel"];

    float min = (double) readings[i]["min"];
    float max = (double) readings[i]["max"];

    JSONVar cfg = readings[i]["widget"];

    Reading<float> *raw = nullptr;
    Reading<float> *scaled = new Reading<float>(name, "ADC", type, new WindowedValue<float, SAMPLE_BACKLOG>(unit, min, max), cfg);

    if ((bool) readings[i]["includeRaw"]) {
      raw = new Reading<float>(name, "ADC", "raw", new WindowedValue<float, SAMPLE_BACKLOG>("counts", 0, MAX_RAW_READING_VALUE));
    }

    numChannels = max(numChannels, (int) channel["number"]);
    channels = new List<ADCChannel>(ADCChannel(channel, scaled, raw), channels);
  }

  return new ADC(rx, tx, numChannels + 1, channels);
}

void ADC::begin(System &system) {
  system.device().attach(this);

  if(!this->sensor.begin()) {
    foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
      c.setError("Failed to initialize the sensor.");
    });
    return;
  }

  foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
    if(!this->sensor.setSamplingInterval(c.index, c.samplingInterval)
       || !this->sensor.setSamplingTime(c.index, c.samplingTime)
       || !this->sensor.setDeltaTime(c.index, c.deltaTime)) {
      c.setError(String("Failed to initialize ADC channel: .") + c.index);
    }
  });

  system.scheduler().spawn("sample ADC", 115,[&](Task *t) {
    system.log().debug("Sampling ADC sensor.");
    this->update();
    t->sleep(ADC_SAMPLE_INTERVAL);
  });
}

void ADC::update() {
  foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
    if (this->sensor.read(c.index)) {
      c.add(this->sensor.getReading(c.index), this->sensor.getVoltage(c.index));
    } else {
      c.setError(String("Failed to read data from ADC channel: ") + c.index);
    }
  });
}

void ADC::connect(Device *d) const {
  foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
    if (c.scaled != nullptr) d->attach(c.scaled);
    if (c.raw != nullptr) d->attach(c.raw);
  });
}
