#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <SPIFFS.h>
#include <FS.h>
#include "APIServer.hpp"
#include "BMPHub.hpp"
#include "DallasTempHub.hpp"
#include "Device.hpp"
#include "DHTHub.hpp"
#include "HTUHub.hpp"
#include "SDSHub.hpp"
#include "Scheduler.hpp"

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int SAMPLE_INTERVAL = 2000; // 10 seconds
const int STATS_INTERVAL = 60000; // 60 seconds
const int TIME_SLICE = 500; // 500 us

const int SDA_PIN = 21;
const int SCL_PIN = 22;

Scheduler scheduler(TIME_SLICE);

void setup(void){
  // CONSOLE
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Serial console initialized");

  // TASK SCHEDULER
  scheduler.begin();
  scheduler.spawn(125, [](Task *t) {
    Serial.print("Free heap memory: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" B");
    Serial.println(scheduler.monitor());
    t->sleep(STATS_INTERVAL);
  });
  Serial.println("Task scheduler initialized");

  // FILE SYSTEM
  SPIFFS.begin();
  Serial.println("File system initialized (" + String(SPIFFS.usedBytes()) + " B / " + String(SPIFFS.totalBytes()) + " B):");

  File dir = SPIFFS.open("/");
  if(!dir.isDirectory()) {
    Serial.println("/ is not a directory!");
  } else {
    File f;
    while (f = dir.openNextFile()) {
      Serial.print("- " + String(f.name()));
      Serial.println(" (" + String(f.size()) + " B)");
      f.close();
    }
  }

  // DEVICE TREE
  Device *device = new Device("53n50rp455w0r0");

  BMPHub *bmp = new BMPHub(SDA_PIN, SCL_PIN, 0x76);
  bmp->begin();
  device->attach(bmp);

  HTUHub *htu = new HTUHub(SDA_PIN, SCL_PIN, 0x40);
  htu->begin();
  device->attach(htu);

  HardwareSerial& sdsSerial(Serial2);
  SDSHub *sds = new SDSHub(sdsSerial);
  sds->begin();
  device->attach(sds);

  // DallasTempHub *dallas = new DallasTempHub(2, 12);
  // dallas->begin();
  // device->attach(dallas);

  // DHTHub *dht11 = new DHTHub(2, DHT11);
  // dht11->begin();
  // device->attach(dht11);

  // DHTHub *dht22 = new DHTHub(2, DHT22);
  // dht22->begin();
  // device->attach(dht22);

  scheduler.spawn(115,[=](Task *t) {
    Serial.println("Sampling sensors.");
    bmp->update();
    htu->update();
    sds->update();
    // dallas->update();
    // dht11->update();
    // dht22->update();

    t->sleep(SAMPLE_INTERVAL);
  });
  Serial.println("Device tree initialized");

  // NETWORK
  WiFi.hostname(device->name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(device->name().c_str(), device->password().c_str());
  scheduler.spawn(125, [](Task *t) {
    static boolean apEnabled = true;

    if (apEnabled) {
      apEnabled = false;
      t->sleep(AP_TIMEOUT);
    } else {
      Serial.println("Disabling access point.");
      WiFi.mode(WIFI_STA);
      t->kill();
    }
  });

  Serial.println("WiFi initialized:");
  Serial.print("- SSID: ");
  Serial.println(device->name());
  Serial.print("- Password: ");
  Serial.println(device->password());
  Serial.print("- IP address: ");
  Serial.println(WiFi.softAPIP());

  // SERVICE DISCOVERY
  MDNS.begin(device->name().c_str());
  Serial.println("mDNS responder initialized");

  // API
  APIServer *server = new APIServer(HTTP_PORT, device, SPIFFS);
  server->begin();
  scheduler.spawn(110, [server](Task *t) {
    server->handleClient();
  });
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("API server initialized");
}

void loop(void) {
  scheduler.run();
}
