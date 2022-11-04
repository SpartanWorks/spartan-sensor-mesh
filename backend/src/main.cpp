#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define FSImplementation SPIFFS // FIXME Deprecated, replace with LittleFS.

const int SDA_PIN = 4;
const int SCL_PIN = 5;
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#define FSImplementation SPIFFS

const int SDA_PIN = 21;
const int SCL_PIN = 22;
#endif

#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <FS.h>
#include <Wire.h>
#include "APIServer.hpp"
#include "System.hpp"

const int TIME_SLICE = 500; // 500 us

System ssn(TIME_SLICE);

const int AP_TIMEOUT = 900000; // 15 minutes

const int SERVICE_QUERY_INTERVAL = 300000; // 5 minutes

void setup(void){
  // SYSTEM
  ssn.begin();
  ssn.log().info("System initialized");

  // FILE SYSTEM
  FSImplementation.begin();
  uint32_t usedBytes = 0;
  uint32_t totalBytes = 0;
#ifdef ESP32
  usedBytes = FSImplementation.usedBytes();
  totalBytes = FSImplementation.totalBytes();
#endif
#ifdef ESP8266
  FSInfo info;
  FSImplementation.info(info);
  usedBytes = info.usedBytes;
  totalBytes = info.totalBytes;
#endif
  ssn.log().info("File system initialized (%ldB / %ldB):", usedBytes, totalBytes);

  File dir = FSImplementation.open("/", "r");
  if(!dir.isDirectory()) {
    ssn.log().warn("/ is not a directory!");
  } else {
    File f;
    while (f = dir.openNextFile()) {
      ssn.log().info("- %s (%ldB)", f.name(), f.size());
      f.close();
    }
  }

  if(FSImplementation.exists("/device_config.json")) {
    File configFile = FSImplementation.open("/device_config.json", "r");
    JSONVar config = JSON.parse(configFile.readString());
    ssn.loadConfig(config);
    configFile.close();
  } else {
    ssn.log().warn("System configuration not found. Please upload a file named `device_config.json` containing the system configuration.");
  }

  Wire.begin(SDA_PIN, SCL_PIN); // FIXME Needed as BMP280 library seems to override it internally.

  // NETWORK
  WiFi.hostname(ssn.device().name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssn.device().name().c_str(), ssn.device().password().c_str());
  ssn.scheduler().spawn("disable AP", 125, [](Task *t) {
    static boolean apEnabled = true;

    if (apEnabled) {
      apEnabled = false;
      t->sleep(AP_TIMEOUT);
    } else {
      ssn.log().info("Disabling access point.");
      WiFi.mode(WIFI_STA);
      t->kill();
    }
  });

  ssn.log().info("WiFi initialized:");
  ssn.log().info("- SSID: %s", ssn.device().name().c_str());
  ssn.log().info("- Password: %s", ssn.device().password().c_str());
  ssn.log().info("- IP address: %s", WiFi.softAPIP().toString().c_str());

  // SERVICE DISCOVERY
  MDNS.begin(ssn.device().name().c_str());
  MDNS.addService("ssn", "tcp", SSN_PORT);
  ssn.scheduler().spawn("scan SSN services", 125, [](Task *t) {
      ssn.log().debug("Querying for SSN services via mDNS.");
      MDNS.queryService("ssn", "tcp");
      t->sleep(SERVICE_QUERY_INTERVAL);
  });
#ifdef ESP8266
  ssn.scheduler().spawn("handle mDNS", 125, [](Task *t) {
    MDNS.update();
  });
#endif
  ssn.log().info("mDNS responder initialized");

  // API
  APIServer *server = new APIServer(ssn.device(), ssn.log(), FSImplementation);
  server->begin();
  ssn.scheduler().spawn("handle API", 110, [server](Task *t) {
    server->handleClient();
  });

  MDNS.addService("http", "tcp", SSN_PORT);
  ssn.log().info("API server initialized");
}

void loop(void) {
  ssn.run();
}