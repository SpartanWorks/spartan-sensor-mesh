#include "System.hpp"
#include "BMPHub.hpp"
#include "DallasTempHub.hpp"
#include "Device.hpp"
#include "DHTHub.hpp"
#include "HTUHub.hpp"
#include "SDSHub.hpp"
#include "MHZHub.hpp"
#include "CCSHub.hpp"
#include "GP2YHub.hpp"

System::System(Timestamp slice):
    l(Log()),
    sched(Scheduler(slice)),
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
    l.debug(sched.monitor());
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
      if(type == "BMPHub") {
        if(bus != "hardware-i2c") {
          l.warn("Bad BMPHub configuration, skipping.");
          continue;
        }

        l.info("Attaching BMPHub with config: ");
        l.info(sensor);

        BMPHub *bmp = new BMPHub(&Wire, (int) conn["address"]);
        bmp->begin(*this);
      } else if (type == "HTUHub") {
        if(bus != "hardware-i2c") {
          l.warn("Bad HTUHub configuration, skipping.");
          continue;
        }

        l.info("Attaching HTUHub with config: ");
        l.info(sensor);

        HTUHub *htu = new HTUHub(&Wire, (int) conn["address"]);
        htu->begin(*this);
      } else if (type == "SDSHub") {
        if(bus != "hardware-uart") {
          l.warn("Bad SDSHub configuration, skipping.");
          continue;
        }

        l.info("Attaching SDSHub with config: ");
        l.info(sensor);

        SDSHub *sds;
        switch((int) conn["number"]) {
#ifdef ESP32
          case 2: {
            HardwareSerial& sdsSerial(Serial2);
            sds = new SDSHub(sdsSerial);
          }
          break;
#endif

          case 1: {
            HardwareSerial& sdsSerial(Serial1);
            sds = new SDSHub(sdsSerial);
          }
          break;

          case 0:
          default: {
            HardwareSerial& sdsSerial(Serial);
            sds = new SDSHub(sdsSerial);
          }
          break;
        }

        sds->begin(*this);
      } else if (type == "MHZHub") {
        if(bus != "software-uart") {
          l.warn("Bad SDSHub configuration, skipping.");
          continue;
        }

        l.info("Attaching MHZHub with config: ");
        l.info(sensor);

        MHZHub *mhz = new MHZHub((int) conn["rx"], (int) conn["tx"]);
        mhz->begin(*this);
      } else if (type == "CCSHub") {
        if(bus != "hardware-i2c") {
          l.warn("Bad CCSHub configuration, skipping.");
          continue;
        }

        l.info("Attaching CCSHub with config: ");
        l.info(sensor);

        CCSHub *ccs = new CCSHub(&Wire, (int) conn["address"]);
        ccs->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "GP2YHub") {
        if(bus != "software-uart") {
          l.warn("Bad GP2YHub configuration, skipping.");
          continue;
        }

        l.info("Attaching GP2YHub with config: ");
        l.info(sensor);

        GP2YHub *gp2y = new GP2YHub((int) conn["rx"], (int) conn["tx"]);
        gp2y->begin(*this);
      } else if (type == "DallasTempHub") {
        if(bus != "dallas-1-wire") {
          l.warn("Bad DallasTempHub configuration, skipping.");
          continue;
        }

        l.info("Attaching DallasTempHub with config: ");
        l.info(sensor);

        DallasTempHub *dallas = new DallasTempHub((int) conn["pin"], (int) sensor["resolution"]);
        dallas->begin(*this);
      } else if (type == "DHTHub") {
        if(bus != "dht11" && bus != "dht22") {
          l.warn("Bad DHTHub configuration, skipping.");
          continue;
        }

        l.info("Attaching DHTHub with config: ");
        l.info(sensor);

        DHTHub *dht = new DHTHub((int) conn["pin"], (bus == "dht22") ? DHT22 : DHT11);
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
