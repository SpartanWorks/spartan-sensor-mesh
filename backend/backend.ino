#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "APIServer.hpp"
#include "DHTSensor.hpp"
#include <FS.h>

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int SAMPLE_INTERVAL = 2000; // 2 seconds
const int SENSOR = 2;

String sensorName = "Sensor-";
const char* sensorPassword = "53n50rp455w0r0";

DHTSensor dht = DHTSensor(SENSOR, DHT22);
APIServer server(HTTP_PORT, &dht, SPIFFS);

void readSensor(uint32_t currTime) {
  static uint32_t lastSampleTime = -SAMPLE_INTERVAL;

  if((currTime - lastSampleTime) < SAMPLE_INTERVAL) {
    return;
  }
  dht.update();
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
  Serial.println("Setting up wifi...");

  sensorName += String(ESP.getChipId(), HEX);
  WiFi.hostname(sensorName.c_str());

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(sensorName.c_str(), sensorPassword);

  Serial.print("Access point on: ");
  Serial.println(sensorName);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  dht.begin();
  readSensor(0);
  Serial.println("Sensor initialized");

  SPIFFS.begin();

  server.begin();
  Serial.println("API server started");

  MDNS.begin(sensorName.c_str());
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("mDNS responder started");
}

void loop(void){
  uint32_t currTime = millis();
  readSensor(currTime);
  timeoutAP(currTime);
  server.handleClient();
}
