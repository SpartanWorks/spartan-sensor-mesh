#include "APIServer.hpp"

APIServer::APIServer(uint16_t port, const Sensor *s): ESP8266WebServer(port), sensor(s)
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
  this->sendHeader("Content-Encoding", "gzip");
  this->send_P(200, PSTR("text/html"), index_html_gz, index_html_gz_len);
}

void APIServer::handleStaticJS() {
  Serial.println("Serving /main.js");
  this->sendHeader("Content-Encoding", "gzip");
  this->send_P(200, PSTR("application/javascript"), main_js_gz, main_js_gz_len);
}

void APIServer::handleStaticCSS() {
  Serial.println("Serving /main.css");
  this->sendHeader("Content-Encoding", "gzip");
  this->send_P(200, PSTR("text/css"), main_css_gz, main_css_gz_len);
}

void APIServer::begin() {
  ESP8266WebServer::begin();

  this->on("/api/config", [this]() { this->handleApiConfig(); });
  this->on("/api/sensor", [this]() { this->handleApiSensor(); });
  this->on("/main.js",    [this]() { this->handleStaticJS(); });
  this->on("/main.css",   [this]() { this->handleStaticCSS(); });
  this->onNotFound(       [this]() { this->handleWildcard(); });
}
