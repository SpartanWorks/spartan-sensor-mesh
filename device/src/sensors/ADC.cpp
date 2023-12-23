#include "ADC.hpp"

ADCChannel::ADCChannel(JSONVar &config, Reading<float> *s, Reading<float> *r):
    scaled(s),
    raw(r)
{
  this->index = (int) config["number"];
  // NOTE All config times are expressed in milliseconds, while this expects microseconds.
  this->samplingInterval = (uint16_t) ((double) config["samplingInterval"] * 1000);
  this->samplingTime = (uint16_t) ((double) config["samplingTime"] * 1000);
  this->deltaTime = (uint16_t) ((double) config["deltaTime"] * 1000);
  this->min = (double) config["min"];
  this->offset = (double) config["offset"];
  this->factor = (double) config["factor"];
  if (config["factorSquared"] == undefined) {
    this->factorSquared = 0;
  } else {
    this->factorSquared = (double) config["factorSquared"];
  }
  if (config["factorCubed"] == undefined) {
    this->factorCubed = 0;
  } else {
    this->factorCubed = (double) config["factorCubed"];
  }
  this->baseline = 0;
}

void ADCChannel::add(uint16_t raw, float volts) {
  if (this->scaled != nullptr) {
    float result = this->factorCubed * volts * volts * volts +
                   this->factorSquared * volts * volts +
                   this->factor * volts +
                   this->offset +
                   this->baseline;

    if (result < this->min) {
      this->baseline += this->min - result;
      return this->add(raw, volts);
    }

    this->scaled->add(result);
  }

  if (this->raw != nullptr) this->raw->add(raw);
}

void ADCChannel::setError(String error) {
  if (this->raw != nullptr) this->raw->setError(error);
}

ADC::ADC(uint8_t numChannels, List<ADCChannel> *channels, uint16_t interval):
    sensor(new BuiltInADC(numChannels)),
    sampleInterval(interval),
    channels(channels)
{}

ADC::ADC(uint8_t rx, uint8_t tx, uint8_t numChannels, List<ADCChannel> *channels, uint16_t interval):
    sensor(new ADCOverUART(rx, tx, numChannels)),
    sampleInterval(interval),
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

  if (this->sensor != nullptr) delete this->sensor;
}

ADC* ADC::create(JSONVar &config) {
  uint16_t interval = (int) config["samplingInterval"];
  JSONVar conn = config["connection"];
  String bus = (const char*) conn["bus"];

  JSONVar readings = config["readings"];

  if (conn == undefined || readings == undefined || ((bus != "software-uart") && (bus != "gpio"))) {
    return nullptr;
  }

  uint8_t numChannels = 0;

  List<ADCChannel> *channels = nullptr;

  for (uint16_t i = 0; i < readings.length(); i++) {
    String type = (const char*) readings[i]["type"];
    String name = (const char*) readings[i]["name"];
    String unit = (const char*) readings[i]["unit"];

    JSONVar channel = readings[i]["channel"];

    float min = (double) channel["min"];
    float max = (double) channel["max"];
    uint16_t window = (int) readings[i]["averaging"];
    JSONVar cfg = readings[i]["widget"];

    Reading<float> *raw = nullptr;
    Reading<float> *scaled = new Reading<float>(name, "ADC", type, new WindowedValue<float>(window, unit, min, max), cfg);

    if (readings[i].hasOwnProperty("includeRaw") && (bool) readings[i]["includeRaw"]) {
      JSONVar rawCfg = readings[i]["rawWidget"];
      raw = new Reading<float>(name, "ADC", "raw", new WindowedValue<float>(window, "", 0, MAX_RAW_READING_VALUE), rawCfg);
    }

    numChannels = max(numChannels, (int) channel["number"]);
    channels = new List<ADCChannel>(ADCChannel(channel, scaled, raw), channels);
  }

  if (bus == "software-uart") {
    uint16_t rx = (int) conn["rx"];
    uint16_t tx = (int) conn["tx"];

    return new ADC(rx, tx, numChannels + 1, channels, interval);
  } else if (bus == "gpio") {
    return new ADC(numChannels + 1, channels, interval);
  } else {
    return nullptr;
  }
}

void ADC::begin(System &system) {
  system.device().attach(this);

  if(!this->sensor->begin()) {
    foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
      c.setError("Failed to initialize the sensor.");
    });
    return;
  }

  foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
    if(!this->sensor->setSamplingInterval(c.index, c.samplingInterval)
       || !this->sensor->setSamplingTime(c.index, c.samplingTime)
       || !this->sensor->setDeltaTime(c.index, c.deltaTime)) {
      c.setError(String("Failed to initialize ADC channel: .") + c.index);
    }
  });

  system.scheduler().spawn("sample ADC", 115,[&](Task *t) {
    system.log().debug("Sampling ADC sensor.");
    this->update();
    t->sleep(this->sampleInterval);
  });
}

void ADC::update() {
  foreach<ADCChannel>(this->channels, [=](ADCChannel c) {
    if (this->sensor->read(c.index)) {
      c.add(this->sensor->getReading(c.index), this->sensor->getVoltage(c.index));
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
