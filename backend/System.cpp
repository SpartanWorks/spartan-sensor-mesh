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
    sched(Scheduler(slice)),
    dev(Device())
{}

void System::begin() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Serial console initialized");

  sched.begin();
  Serial.println("Scheduler initialized");

  sched.spawn("system monitor", 125, [&](Task *t) {
    Serial.print("Free heap memory: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" B");
    Serial.println(sched.monitor());
    t->sleep(STATS_INTERVAL);
  });
}

bool System::loadConfig(JSONVar &config) {
  if(JSON.typeof(config) == "undefined" || JSON.typeof(config["sensors"]) != "array") {
    // TODO Add more validation.
    Serial.print("Invalid configuration JSON specified:");
    Serial.println(JSON.stringify(config));
    return false;
  }

  String model = (const char*) config["model"];
  String group = (const char*) config["group"];
  String name = (const char*) config["name"];
  String password = (const char*) config["password"];

  this->dev = Device(model, group, name, password);

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
          Serial.println("Bad BMPHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching BMPHub with config: ");
        Serial.println(JSON.stringify(sensor));

        BMPHub *bmp = new BMPHub(&Wire, (int) conn["address"]);
        bmp->begin(*this);
      } else if (type == "HTUHub") {
        if(bus != "hardware-i2c") {
          Serial.println("Bad HTUHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching HTUHub with config: ");
        Serial.println(JSON.stringify(sensor));

        HTUHub *htu = new HTUHub(&Wire, (int) conn["address"]);
        htu->begin(*this);
      } else if (type == "SDSHub") {
        if(bus != "hardware-uart") {
          Serial.println("Bad SDSHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching SDSHub with config: ");
        Serial.println(JSON.stringify(sensor));

        SDSHub *sds;
        switch((int) conn["number"]) {
          case 2: {
            HardwareSerial& sdsSerial(Serial2);
            sds = new SDSHub(sdsSerial);
          }
            break;
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
          Serial.println("Bad SDSHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching MHZHub with config: ");
        Serial.println(JSON.stringify(sensor));

        MHZHub *mhz = new MHZHub((int) conn["rx"], (int) conn["tx"]);
        mhz->begin(*this);
      } else if (type == "CCSHub") {
        if(bus != "hardware-i2c") {
          Serial.println("Bad CCSHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching CCSHub with config: ");
        Serial.println(JSON.stringify(sensor));

        CCSHub *ccs = new CCSHub(&Wire, (int) conn["address"]);
        ccs->begin(*this);
        // htu->compensate(ccs); // TODO
      } else if (type == "GP2YHub") {
        if(bus != "software-uart") {
          Serial.println("Bad GP2YHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching GP2YHub with config: ");
        Serial.println(JSON.stringify(sensor));

        GP2YHub *gp2y = new GP2YHub((int) conn["rx"], (int) conn["tx"]);
        gp2y->begin(*this);
      } else if (type == "DallasTempHub") {
        if(bus != "dallas-1-wire") {
          Serial.println("Bad DallasTempHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching DallasTempHub with config: ");
        Serial.println(JSON.stringify(sensor));

        DallasTempHub *dallas = new DallasTempHub((int) conn["pin"], (int) sensor["resolution"]);
        dallas->begin(*this);
      } else if (type == "DHTHub") {
        if(bus != "dht11" && bus != "dht22") {
          Serial.println("Bad DHTHub configuration, skipping.");
          continue;
        }

        Serial.print("Attaching DHTHub with config: ");
        Serial.println(JSON.stringify(sensor));

        DHTHub *dht = new DHTHub((int) conn["pin"], (bus == "dht22") ? DHT22 : DHT11);
        dht->begin(*this);
      } else {
        Serial.print("Skipping unrecognized sensor ");
        Serial.print(type);
        Serial.println(".");
      }
    } else {
      Serial.print("Skipping disabled sensor ");
      Serial.print(type);
      Serial.println(".");
    }
  }

  Serial.println("Device tree initialized");

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
