#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Wire.h>
#include "APIServer.hpp"
#include "System.hpp"

const int TIME_SLICE = 500; // 500 us

System ssn(TIME_SLICE);

const int SDA_PIN = 21;
const int SCL_PIN = 22;

const int AP_TIMEOUT = 900000; // 15 minutes

const int SERVICE_QUERY_INTERVAL = 300000; // 5 minutes

void setup(void){
  // SYSTEM
  ssn.begin();
  Serial.println("System initialized");

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

  if(SPIFFS.exists("/device_config.json")) {
    File configFile = SPIFFS.open("/device_config.json", "r");
    JSONVar config = JSON.parse(configFile.readString());
    ssn.loadConfig(config);
    configFile.close();
  } else {
    Serial.println("System configuration not found. Please upload a file named `device_config.json` containing the system configuration.");
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
      Serial.println("Disabling access point.");
      WiFi.mode(WIFI_STA);
      t->kill();
    }
  });

  Serial.println("WiFi initialized:");
  Serial.print("- SSID: ");
  Serial.println(ssn.device().name());
  Serial.print("- Password: ");
  Serial.println(ssn.device().password());
  Serial.print("- IP address: ");
  Serial.println(WiFi.softAPIP());

  // SERVICE DISCOVERY
  MDNS.begin(ssn.device().name().c_str());
  MDNS.addService("ssn", "tcp", SSN_PORT);
  ssn.scheduler().spawn("scan SSN services", 125, [](Task *t) {
      Serial.println("Querying for SSN services via mDNS.");
      MDNS.queryService("ssn", "tcp");
      t->sleep(SERVICE_QUERY_INTERVAL);
  });
  Serial.println("mDNS responder initialized");

  // API
  APIServer *server = new APIServer(&ssn.device(), SPIFFS);
  server->begin();
  ssn.scheduler().spawn("handle API", 110, [server](Task *t) {
    server->handleClient();
  });

  MDNS.addService("http", "tcp", SSN_PORT);
  Serial.println("API server initialized");
}

void loop(void) {
  ssn.run();
}
