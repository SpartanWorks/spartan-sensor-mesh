#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHTSensor.hpp"
#include "index.html.gz.h"
#include "main.css.gz.h"
#include "main.js.gz.h"

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int CONNECTION_TIMEOUT = 20000; // 20 seconds
const int SAMPLE_INTERVAL = 2000; // 2 seconds
const int SENSOR = 2;
const int LED = 13;

String sensorName = "Sensor-";
const char* sensorPassword = "53n50rp455w0r0";

ESP8266WebServer server(HTTP_PORT);
DHTSensor dht = DHTSensor(SENSOR, DHT11);

bool waitForConnection(uint32_t timeout) {
  uint32_t i = 0;
  while((WiFi.status() != WL_CONNECTED) && (i < timeout)) {
    Serial.print(".");
    delay(500);
    i += 500;
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection failed.");
    return false;
  }

  return true;
}

bool connect(const char *ssid, const char *password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  waitForConnection(CONNECTION_TIMEOUT);

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void handleIndex() {
  digitalWrite(LED, 1);
  Serial.println("Serving *");
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, PSTR("text/html"), index_html_gz, index_html_gz_len);
  digitalWrite(LED, 0);
}

void handleJS() {
  digitalWrite(LED, 1);
  Serial.println("Serving /main.js");
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, PSTR("application/javascript"), main_js_gz, main_js_gz_len);
  digitalWrite(LED, 0);
}

void handleCSS() {
  digitalWrite(LED, 1);
  Serial.println("Serving /main.css");
  server.sendHeader("Content-Encoding", "gzip");
  server.send_P(200, PSTR("text/css"), main_css_gz, main_css_gz_len);
  digitalWrite(LED, 0);
}

void handleConfig() {
  digitalWrite(LED, 1);
  Serial.println("Serving /api/config");

  String ssid, pass;

  for(uint8_t i = 0; i < server.args(); ++i) {
    String name = server.argName(i);
    String arg = server.arg(i);
    if (name == "ssid") {
      ssid = arg;
    } else if (name == "pass") {
      pass = arg;
    }
  }

  if(connect(ssid.c_str(), pass.c_str())) {
    server.send(200, "text/plain", "OK!");
  } else {
    server.send(401, "text/plain", "Not authorized!");
  }
  digitalWrite(LED, 0);
}

void handleSensor() {
  digitalWrite(LED, 1);
  Serial.println("Serving /api/sensor");

  server.send(200, "application/json", dht.toJSON());
  digitalWrite(LED, 0);
}

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
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);

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

  MDNS.begin(sensorName.c_str());
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("mDNS responder started");

  server.on("/api/config", handleConfig);
  server.on("/api/sensor", handleSensor);
  server.on("/main.js", handleJS);
  server.on("/main.css", handleCSS);
  server.onNotFound(handleIndex);

  server.begin();
  Serial.println("HTTP server started");

  dht.begin();
  readSensor(0);
  Serial.println("Sensor initialized");
}

void loop(void){
  uint32_t currTime = millis();
  readSensor(currTime);
  timeoutAP(currTime);
  server.handleClient();
}
