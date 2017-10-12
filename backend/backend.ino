#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>
#include "APIServer.hpp"
#include "BMPHub.hpp"
#include "DHTHub.hpp"
#include "DallasTempHub.hpp"
#include "Device.hpp"
#include "Scheduler.hpp"

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int SAMPLE_INTERVAL = 2000; // 2 seconds
const int STATS_INTERVAL = 10000; // 10 seconds

Scheduler scheduler;

void setup(void){
  // CONSOLE
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Serial console initialized");

  // TASK SCHEDULER
  scheduler.begin();
  scheduler.spawn(127, [](Task *t) {
    Serial.println(scheduler.monitor());
    t->sleep(STATS_INTERVAL);
  });
  Serial.println("Task scheduler initialized");

  // FILE SYSTEM
  SPIFFS.begin();
  FSInfo info;
  SPIFFS.info(info);
  Serial.println("File system initialized (" + String(info.usedBytes) + " B / " + String(info.totalBytes) + " B):");

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print("- " + dir.fileName());
    File f = dir.openFile("r");
    Serial.println(" (" + String(f.size()) + " B)");
    f.close();
  }

  // DEVICE TREE
  BMPHub *hub = new BMPHub(2, 0, 0x76);
  // DallasTempHub hub = new DallasTempHub(2, 12);
  // DHTHub hub = new DHTHub(2, DHT11);
  // DHTHub hub = new DHTHub(2, DHT22);
  Device *device = new Device("53n50rp455w0r0");

  hub->begin();
  device->attach(hub);
  scheduler.spawn(127,[hub](Task *t) {
    Serial.println("Sampling sensors.");
    hub->update();
    t->sleep(SAMPLE_INTERVAL);
  });
  Serial.println("Device tree initialized");

  // NETWORK
  WiFi.hostname(device->name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(device->name().c_str(), device->password().c_str());
  scheduler.spawn(127, [](Task *t) {
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
  scheduler.spawn(16, [server](Task *t) {
    server->handleClient();
  });
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("API server initialized");
}

void loop(void) {
  scheduler.run();
}
