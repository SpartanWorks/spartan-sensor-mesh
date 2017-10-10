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

BMPHub hub(2, 0, 0x76);
// DallasTempHub hub(2, 12);
// DHTHub hub(2, DHT11);
// DHTHub hub(2, DHT22);
Device device("53n50rp455w0r0");
APIServer server(HTTP_PORT, device, SPIFFS);

Scheduler scheduler;

void readSensor() {
  static uint32_t lastSampleTime = -SAMPLE_INTERVAL;

  uint32_t currTime = millis();

  if((currTime - lastSampleTime) < SAMPLE_INTERVAL) {
    return;
  }

  hub.update();
  lastSampleTime = currTime;
}

void timeoutAP() {
  static boolean apEnabled = true;

  uint32_t currTime = millis();

  if(apEnabled && currTime > AP_TIMEOUT) {
    Serial.println("Disabling access point.");
    WiFi.mode(WIFI_STA);
    apEnabled = false;
  }
}

void setup(void){
  Serial.begin(115200);
  Serial.println("");

  WiFi.hostname(device.name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(device.name().c_str(), device.password().c_str());

  Serial.println("WiFi initialized:");
  Serial.print("- SSID: ");
  Serial.println(device.name());
  Serial.print("- Password: ");
  Serial.println(device.password());
  Serial.print("- IP address: ");
  Serial.println(WiFi.softAPIP());

  SPIFFS.begin();
  FSInfo info;
  SPIFFS.info(info);
  Serial.println("FS initialized (" + String(info.usedBytes) + " B / " + String(info.totalBytes) + " B):");

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print("- " + dir.fileName());
    File f = dir.openFile("r");
    Serial.println(" (" + String(f.size()) + " B)");
    f.close();
  }

  hub.begin();
  device.attach(&hub);
  readSensor();
  Serial.println("Device initialized");

  server.begin();
  Serial.println("API server initialized");

  MDNS.begin(device.name().c_str());
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("mDNS responder initialized");

  scheduler.begin();
  scheduler.spawn(readSensor);
  scheduler.spawn(timeoutAP);
  scheduler.spawn([&server]() {
    server.handleClient();
  });
  Serial.println("Task scheduler initialized");
}

void loop(void) {
  scheduler.run();
}
