#include "System.hpp"
#include "Device.hpp"
#include "sensors/BMP.hpp"
#include "sensors/DallasTemp.hpp"
#include "sensors/DHT.hpp"
#include "sensors/HTU.hpp"
#include "sensors/SDS.hpp"
#include "sensors/MHZ.hpp"
#include "sensors/CCS.hpp"
#include "sensors/SGP.hpp"
#include "sensors/ADC.hpp"

System::System(Timestamp slice):
    l(Log()),
    sched(Scheduler(slice, l)),
    dev(Device())
{}

bool System::begin(JSONVar &config) {
  if(JSON.typeof(config) == "undefined") {
    l.error("Malformed JSON configuration file.");
    return false;
  }

  if(!config.hasOwnProperty("log")) {
    l.error("Malformed JSON configuration file. Missing `log` property:");
    l.error(config);
    return false;
  }

  JSONVar logConfig = config["log"];
  l.begin(logConfig);
  l.info("Log initialized:");
  l.info(logConfig);

  sched.begin();
  l.info("Scheduler initialized");

  sched.spawn("system monitor", 125, [&](Task *t) {
    l.debug("Free heap memory: %dB", ESP.getFreeHeap());
    sched.monitor();
    t->sleep(STATS_INTERVAL);
  });

  String model = (const char*) config["model"];
  String group = (const char*) config["group"];
  String name = (const char*) config["name"];
  String password = (const char*) config["password"];

  this->dev = Device(model, group, name, password);

  if(JSON.typeof(config["sensors"]) != "array") {
    l.error("Malformed JSON configuration file. Missing `sensors` property:");
    l.error(config);
    return false;
  }

  JSONVar sensors = config["sensors"];
  uint16_t numSensors = sensors.length();

  for(uint16_t i = 0; i < numSensors; i++) {
    JSONVar sensor = sensors[i];
    String type = (const char*) sensor["type"];

    if((bool) sensor["enabled"]) {
      if(type == "BMP") {
        l.info("Attaching BMP with config: ");
        l.info(sensor);

        BMP *bmp = BMP::create(sensor);

        if(bmp == nullptr) {
          l.warn("Bad BMP configuration, skipping.");
          continue;
        }

        bmp->begin(*this);
      } else if (type == "HTU") {
        l.info("Attaching HTU with config: ");
        l.info(sensor);

        HTU *htu = HTU::create(sensor);

        if(htu == nullptr) {
          l.warn("Bad HTU configuration, skipping.");
          continue;
        }

        htu->begin(*this);
      } else if (type == "SDS") {
        l.info("Attaching SDS with config: ");
        l.info(sensor);

        SDS *sds = SDS::create(sensor);

        if(sds == nullptr) {
          l.warn("Bad SDS configuration, skipping.");
          continue;
        }

        sds->begin(*this);
      } else if (type == "MHZ") {
        l.info("Attaching MHZ with config: ");
        l.info(sensor);

        MHZ *mhz = MHZ::create(sensor);

        if(mhz == nullptr) {
          l.warn("Bad SDS configuration, skipping.");
          continue;
        }

        mhz->begin(*this);
      } else if (type == "CCS") {
        l.info("Attaching CCS with config: ");
        l.info(sensor);

        CCS *ccs = CCS::create(sensor);

        if(ccs == nullptr) {
          l.warn("Bad CCS configuration, skipping.");
          continue;
        }

        ccs->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "SGP") {
        l.info("Attaching SGP with config: ");
        l.info(sensor);

        SGP *sgp = SGP::create(sensor);

        if(sgp == nullptr) {
          l.warn("Bad SGP configuration, skipping.");
          continue;
        }

        sgp->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "ADC") {
        l.info("Attaching ADC with config: ");
        l.info(sensor);

        ADC *adc = ADC::create(sensor);

        if(adc == nullptr) {
          l.warn("Bad ADC configuration, skipping.");
          continue;
        }

        adc->begin(*this);
      } else if (type == "DallasTemp") {
        l.info("Attaching DallasTemp with config: ");
        l.info(sensor);

        DallasTemp *dallas = DallasTemp::create(sensor);

        if(dallas == nullptr) {
          l.warn("Bad DallasTemp configuration, skipping.");
          continue;
        }

        dallas->begin(*this);
      } else if (type == "DHT") {
        l.info("Attaching DHT with config: ");
        l.info(sensor);
        ssn::DHT *dht = ssn::DHT::create(sensor);

        if(dht == nullptr) {
          l.warn("Bad DHT configuration, skipping.");
          continue;
        }

        dht->begin(*this);
      } else {
        l.warn("Skipping unrecognized sensor %s.", type.c_str());
      }
    } else {
      l.info("Skipping disabled sensor %s.", type.c_str());
    }
  }

  l.info("Device tree initialized");

  return true;
}

void System::run() {
  sched.run();
}

Scheduler& System::scheduler() {
  return sched;
}

Device& System::device() {
  return dev;
}

Log& System::log() {
  return l;
}
