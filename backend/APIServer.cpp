#include "APIServer.hpp"

APIServer::APIServer(uint16_t port, const Sensor *s, FS &fs): ESP8266WebServer(port), sensor(s), files(fs)
{}

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

bool connect(const String ssid, const String password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());
  waitForConnection(WIFI_CONNECTION_TIMEOUT);

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void APIServer::handleApiConfig() {
  Serial.println("Serving /api/config");

  String ssid, pass;

  for(uint8_t i = 0; i < this->args(); ++i) {
    String name = this->argName(i);
    String arg = this->arg(i);
    if (name == "ssid") {
      ssid = arg;
    } else if (name == "pass") {
      pass = arg;
    }
  }

  if(connect(ssid, pass)) {
    this->send(200, "text/plain", "OK!");
  } else {
    this->send(401, "text/plain", "Not authorized!");
  }
}

void APIServer::handleApiSensor() {
  Serial.println("Serving /api/sensor");
  this->send(200, "application/json", this->sensor->toJSON());
}

void APIServer::handleWildcard() {
  Serial.println("Serving *");
  File f = this->files.open("/index.html.gz", "r");

  if(!f) {
    this->send(500, "application/json", "{\"error\":\"Internal server error.\"}");
  }

  this->streamFile(f, "text/html");
  f.close();
}

void APIServer::begin() {
  ESP8266WebServer::begin();

  this->on("/api/config",       [this]() { this->handleApiConfig(); });
  this->on("/api/sensor",       [this]() { this->handleApiSensor(); });
  this->serveStatic("/static/", this->files, "/", "max-age=86400");
  this->onNotFound(             [this]() { this->handleWildcard(); });
}
