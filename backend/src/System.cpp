#include "System.hpp"
#include "Device.hpp"
#include "sensors/BMP.hpp"
#include "sensors/DallasTemp.hpp"
#include "sensors/DHT.hpp"
#include "sensors/HTU.hpp"
#include "sensors/SDS.hpp"
#include "sensors/MHZ.hpp"
#include "sensors/CCS.hpp"
#include "sensors/GP2Y.hpp"
#include "sensors/SGP.hpp"

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
    JSONVar conn = sensor["connection"];
    String bus = (const char*) conn["bus"];

    if((bool) sensor["enabled"]) {
      if(type == "BMP") {
        if(bus != "hardware-i2c") {
          l.warn("Bad BMP configuration, skipping.");
          continue;
        }

        l.info("Attaching BMP with config: ");
        l.info(sensor);

        BMP *bmp = new BMP(&Wire, (int) conn["address"]);
        bmp->begin(*this);
      } else if (type == "HTU") {
        if(bus != "hardware-i2c") {
          l.warn("Bad HTU configuration, skipping.");
          continue;
        }

        l.info("Attaching HTU with config: ");
        l.info(sensor);

        HTU *htu = new HTU(&Wire, (int) conn["address"]);
        htu->begin(*this);
      } else if (type == "SDS") {
        if(bus != "hardware-uart") {
          l.warn("Bad SDS configuration, skipping.");
          continue;
        }

        l.info("Attaching SDS with config: ");
        l.info(sensor);

        SDS *sds;
        switch((int) conn["number"]) {
#ifdef ESP32
          case 2: {
            HardwareSerial& sdsSerial(Serial2);
            sds = new SDS(sdsSerial);
          }
          break;
#endif

          case 1: {
            HardwareSerial& sdsSerial(Serial1);
            sds = new SDS(sdsSerial);
          }
          break;

          case 0:
          default: {
            HardwareSerial& sdsSerial(Serial);
            sds = new SDS(sdsSerial);
          }
          break;
        }

        sds->begin(*this);
      } else if (type == "MHZ") {
        if(bus != "software-uart") {
          l.warn("Bad SDS configuration, skipping.");
          continue;
        }

        l.info("Attaching MHZ with config: ");
        l.info(sensor);

        MHZ *mhz = new MHZ((int) conn["rx"], (int) conn["tx"]);
        mhz->begin(*this);
      } else if (type == "CCS") {
        if(bus != "hardware-i2c") {
          l.warn("Bad CCS configuration, skipping.");
          continue;
        }

        l.info("Attaching CCS with config: ");
        l.info(sensor);

        CCS *ccs = new CCS(&Wire, (int) conn["address"]);
        ccs->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "SGP") {
        if(bus != "hardware-i2c") {
          l.warn("Bad SGP configuration, skipping.");
          continue;
        }

        l.info("Attaching SGP with config: ");
        l.info(sensor);

        SGP *sgp = new SGP(&Wire, (int) conn["address"]);
        sgp->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "GP2Y") {
        if(bus != "software-uart") {
          l.warn("Bad GP2Y configuration, skipping.");
          continue;
        }

        l.info("Attaching GP2Y with config: ");
        l.info(sensor);

        GP2Y *gp2y = new GP2Y((int) conn["rx"], (int) conn["tx"]);
        gp2y->begin(*this);
      } else if (type == "DallasTemp") {
        if(bus != "dallas-1-wire") {
          l.warn("Bad DallasTemp configuration, skipping.");
          continue;
        }

        l.info("Attaching DallasTemp with config: ");
        l.info(sensor);

        DallasTemp *dallas = new DallasTemp((int) conn["pin"], (int) sensor["resolution"]);
        dallas->begin(*this);
      } else if (type == "DHT") {
        if(bus != "dht11" && bus != "dht22") {
          l.warn("Bad DHT configuration, skipping.");
          continue;
        }

        l.info("Attaching DHT with config: ");
        l.info(sensor);

        ssn::DHT *dht = new ssn::DHT((int) conn["pin"], (bus == "dht22") ? DHT22 : DHT11);
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
