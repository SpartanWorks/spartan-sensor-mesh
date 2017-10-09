#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "APIServer.hpp"
#include "BMPHub.hpp"
#include "DHTHub.hpp"
#include "DallasTempHub.hpp"
#include "Device.hpp"
#include <FS.h>

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int SAMPLE_INTERVAL = 2000; // 2 seconds

BMPHub bmp(2, 0, 0x76);
Device device("53n50rp455w0r0");
APIServer server(HTTP_PORT, device, SPIFFS);

void readSensor(uint32_t currTime) {
  static uint32_t lastSampleTime = -SAMPLE_INTERVAL;

  if((currTime - lastSampleTime) < SAMPLE_INTERVAL) {
    return;
  }

  bmp.update();
  lastSampleTime = currTime;
}

void timeoutAP(uint32_t currTime) {
  static boolean apEnabled = true;

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

  bmp.begin();
  device.attach(&bmp);
  readSensor(0);
  Serial.println("Device initialized");

  server.begin();
  Serial.println("API server initialized");

  MDNS.begin(device.name().c_str());
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("mDNS responder initialized");
}

void loop(void){
  uint32_t currTime = millis();
  readSensor(currTime);
  timeoutAP(currTime);
  server.handleClient();
}
