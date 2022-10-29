#include <Arduino.h>
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
#include "MHZHub.hpp"
#include "CCSHub.hpp"
#include "GP2YHub.hpp"
#include "Scheduler.hpp"

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int SAMPLE_INTERVAL = 2000; // 10 seconds
const int STATS_INTERVAL = 60000; // 60 seconds
const int TIME_SLICE = 500; // 500 us

const int SDA_PIN = 21;
const int SCL_PIN = 22;

const int MHZ_RX = 13;
const int MHZ_TX = 12;

const int GP2Y_RX = 14;
const int GP2Y_TX = 15;

const int MHZ_WARMUP_TIMEOUT = 1200000; // 20 minutes
const int CCS_WARMUP_TIMEOUT = 1200000; // 20 minutes

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

  // COMMUNICATION BUSSES
  HardwareSerial& sdsSerial(Serial2);

  // DEVICE TREE
  Device *device = new Device("53n50rp455w0r0");

  BMPHub *bmp = new BMPHub(&Wire, 0x76);
  bmp->begin();
  device->attach(bmp);

  HTUHub *htu = new HTUHub(&Wire, 0x40);
  htu->begin();
  device->attach(htu);

  SDSHub *sds = new SDSHub(sdsSerial);
  sds->begin();
  device->attach(sds);

  MHZHub *mhz = new MHZHub(MHZ_RX, MHZ_TX);
  mhz->begin();
  device->attach(mhz);

  CCSHub *ccs = new CCSHub(&Wire, 0x5A);
  ccs->begin();
  device->attach(ccs);
  htu->compensate(ccs);
  
  GP2YHub *gp2y = new GP2YHub(GP2Y_RX, GP2Y_TX);
  gp2y->begin();
  device->attach(gp2y);

  // Other available sensors:
  // DallasTempHub *dallas = new DallasTempHub(2, 12);
  // dallas->begin();
  // device->attach(dallas);

  // DHTHub *dht11 = new DHTHub(2, DHT11);
  // dht11->begin();
  // device->attach(dht11);

  // DHTHub *dht22 = new DHTHub(2, DHT22);
  // dht22->begin();
  // device->attach(dht22);

  Wire.begin(SDA_PIN, SCL_PIN); // FIXME Needed as BMP280 library seems to override it.

  // SAMPLING TASKS

  scheduler.spawn("sample HTU", 115,[=](Task *t) {
    Serial.println("Sampling HTU hub.");
    htu->update();
    t->sleep(SAMPLE_INTERVAL);
  });

  scheduler.spawn("sample BMP", 115,[=](Task *t) {
    Serial.println("Sampling BMP hub.");
    bmp->update();
    t->sleep(SAMPLE_INTERVAL);
  });

  scheduler.spawn("sample SDS", 115,[=](Task *t) {
    Serial.println("Sampling SDS hub.");
    sds->update();
    t->sleep(SAMPLE_INTERVAL);
  });

  scheduler.spawn("sample MHZ", 115,[=](Task *t) {
    Serial.println("Sampling MHZ hub.");
    mhz->update();
    t->sleep(SAMPLE_INTERVAL);
  });
  scheduler.spawn("reset MHZ", 125,[=](Task *t) {
    static boolean mhzWarmup = true;

    if (mhzWarmup) {
      mhzWarmup = false;
      t->sleep(MHZ_WARMUP_TIMEOUT);
    } else {
      Serial.println("Resetting MHZ hub after a warmup.");
      mhz->reset();
      t->kill();
    }
  });

  scheduler.spawn("sample CCS", 115,[=](Task *t) {
    Serial.println("Sampling CCS hub.");
    ccs->update();
    t->sleep(SAMPLE_INTERVAL);
  });
  scheduler.spawn("reset CCS", 125,[=](Task *t) {
    static boolean ccsWarmup = true;

    if (ccsWarmup) {
      ccsWarmup = false;
      t->sleep(CCS_WARMUP_TIMEOUT);
    } else {
      Serial.println("Resetting CCS hub after a warmup.");
      ccs->reset();
      t->kill();
    }
  });
  scheduler.spawn("sample GP2Y", 115,[=](Task *t) {
    Serial.println("Sampling GP2Y hub.");
    gp2y->update();
    t->sleep(SAMPLE_INTERVAL);
  });
  Serial.println("Device tree initialized");

  // NETWORK
  WiFi.hostname(device->name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(device->name().c_str(), device->password().c_str());
  scheduler.spawn("disable AP", 125, [](Task *t) {
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
  scheduler.spawn("handle API", 110, [server](Task *t) {
    server->handleClient();
  });
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("API server initialized");
}

void loop(void) {
  scheduler.run();
}
