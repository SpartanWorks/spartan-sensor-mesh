#include "APIServer.hpp"

APIServer::APIServer(uint16_t port, const Device *d, FS &fs): WebServer(port), device(d), files(fs) {
}

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

const char * LOWER_CASE_AUTHORIZATION_HEADER = "authorization";
const char *AUTHORIZATION_HEADER = "Authorization";

void APIServer::handleOptions() {
  Serial.println("Serving OPTIONS");
  this->sendHeader("Access-Control-Allow-Headers", AUTHORIZATION_HEADER);
  this->send(200, "application/json", "{\"status\":\"ok}");
}

void APIServer::handleApiLogin() {
  Serial.println("Serving /api/login");

  // FIXME Some browsers send lowercase authorization header.
  const char *original = AUTHORIZATION_HEADER;

  if(!this->hasHeader(original)) {
    AUTHORIZATION_HEADER = LOWER_CASE_AUTHORIZATION_HEADER;
  }

  bool authorized = this->authenticate(this->device->name().c_str(), this->device->password().c_str());

  AUTHORIZATION_HEADER = original;
  return authorized ? this->send(200, "application/json", "{\"status\":\"ok\"}") : this->requestAuthentication();
}

void APIServer::handleApiConfig() {
  Serial.println("Serving /api/config");

  if(!this->authenticate(this->device->name().c_str(), this->device->password().c_str())) {
    this->send(401, "application/json", "{\"error\":\"Unauthorized.\"}");
  }

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
    this->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    this->send(403, "application/json", "{\"error\":\"Invalid credentials.\"}");
  }
}

void APIServer::handleApiData() {
  Serial.println("Serving /api/data");
  this->send(200, "application/json", this->device->toJSON());
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
  WebServer::begin();

  // FIXME Some browsers send lowercase authorization header.
  const char *headers[] = { LOWER_CASE_AUTHORIZATION_HEADER };
  this->collectHeaders(headers, sizeof(headers)/sizeof(headers[0]));

  this->on("/api/login" , HTTP_OPTIONS, [this]() { this->handleOptions(); });
  this->on("/api/login" , HTTP_GET,     [this]() { this->handleApiLogin(); });
  this->on("/api/config",               [this]() { this->handleApiConfig(); });
  this->on("/api/data"  ,               [this]() { this->handleApiData(); });
  this->onNotFound(                     [this]() { this->handleWildcard(); });
  this->serveStatic("/static/",         this->files, "/", "max-age=86400");
}
