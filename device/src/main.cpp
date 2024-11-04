#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

const int SDA_PIN = 4;
const int SCL_PIN = 5;
#endif

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>

const int SDA_PIN = 21;
const int SCL_PIN = 22;
#endif

#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "APIServer.hpp"
#include "System.hpp"
#include <ElegantOTA.h>
#include <LittleFS.h>

#include "sensors/SSN.hpp"

const int TIME_SLICE = 500; // 500 us

System ssn(TIME_SLICE);

const int AP_TIMEOUT = 900000; // 15 minutes

const int SERVICE_QUERY_INTERVAL = 300000; // 5 minutes

void setup(void){
  // FILE SYSTEM
  LittleFS.begin();
  uint32_t usedBytes = SSN::usedFSSize();
  uint32_t totalBytes = SSN::totalFSSize();

  // SYSTEM
  if(LittleFS.exists("/device_config.json")) {
    File configFile = LittleFS.open("/device_config.json", "r");
    JSONVar config = JSON.parse(configFile.readString());
    ssn.begin(config);
    configFile.close();
    ssn.log().info("System initialized");
  } else {
    ssn.begin(undefined);
    ssn.log().warn("System configuration not found. Please upload a file named `device_config.json` containing the system configuration.");
  }

  ssn.log().info("File system initialized (%ldB / %ldB):", usedBytes, totalBytes);

  File dir = LittleFS.open("/", "r");
  if(!dir.isDirectory()) {
    ssn.log().warn("/ is not a directory!");
  } else {
    File f;
    while (f = dir.openNextFile()) {
      ssn.log().info("- %s (%ldB)", f.name(), f.size());
      f.close();
    }
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

  // API
  APIServer *server = new APIServer(ssn, LittleFS);
  server->begin();
  ssn.scheduler().spawn("handle API", 100, [server](Task *t) {
    server->handleClient();
  });

  // OTA
  ElegantOTA.begin(server, ssn.device().name().c_str(), ssn.device().password().c_str());
  ElegantOTA.onStart([]() {
    ssn.log().info("OTA update started!");
  });
  ElegantOTA.onProgress([](size_t current, size_t total) {
    ssn.log().info("OTA update progress: %u bytes", current);
  });
  ElegantOTA.onEnd([](bool success) {
    if (success) {
      ssn.log().info("OTA update succeeded!");
    } else {
      ssn.log().info("OTA update failed!");
    }
  });
  ssn.scheduler().spawn("handle OTA", 100, [server](Task *t) {
    ElegantOTA.loop();
  });

  ssn.log().info("API server initialized");

  // Service discovery
  ssn.mesh().begin(ssn.device().name(), WiFi.localIP().toString(), SSM_PORT);

  ssn.scheduler().spawn("scan for SSM services", 125, [](Task *t) {
    ssn.log().debug("Querying for SSM services via mDNS.");
    ssn.mesh().scan();
    t->sleep(SERVICE_QUERY_INTERVAL);
  });
  ssn.scheduler().spawn("handle mesh", 125, [](Task *t) {
    ssn.mesh().update();
  });

  ssn.log().info("Mesh responder initialized");
}

void loop(void) {
  ssn.run();
}
