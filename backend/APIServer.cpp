#include "APIServer.hpp"

APIServer::APIServer(const Device *d, FS &fs): WebServer(SSN_PORT), device(d), files(fs) {
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
  bool result = waitForConnection(WIFI_CONNECTION_TIMEOUT);

  if(result) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  return result;
}

void APIServer::handleOptions() {
  Serial.println("Serving OPTIONS");
  this->sendHeader(ALLOWED_HEADER, AUTHORIZATION_HEADER);
  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);
  this->send(200, APPLICATION_JSON, STATUS_OK);
}

void APIServer::handleApiLogin() {
  Serial.println("Serving /api/login");

  bool authorized = this->authenticate(this->device->name().c_str(), this->device->password().c_str());

  return authorized ? this->send(200, APPLICATION_JSON, STATUS_OK) : this->requestAuthentication();
}

void APIServer::handleApiConfig() {
  Serial.println("Serving /api/config");

  if(!this->authenticate(this->device->name().c_str(), this->device->password().c_str())) {
    this->send(401, APPLICATION_JSON, UNAUTHORIZED);
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
    Serial.println("Saving WiFi configuration...");
    File f = this->files.open(WIFI_CONFIG_FILE, "w");
    f.write((const uint8_t*)ssid.c_str(), ssid.length());
    f.write('\n');
    f.write((const uint8_t*)pass.c_str(), pass.length());
    f.write('\n');
    f.close();

    this->send(200, APPLICATION_JSON, STATUS_OK);
  } else {
    this->send(403, APPLICATION_JSON, INVALID_CREDENTIALS);
  }
}

void APIServer::handleApiData() {
  Serial.println("Serving /api/data");
  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);
  this->send(200, APPLICATION_JSON, this->device->toJSON());
}

void APIServer::handleApiMesh() {
  Serial.println("Serving /api/mesh");
  JSONVar mesh;

  JSONVar self;

  self["hostname"] = this->device->name();
  self["ip"] = WiFi.localIP().toString();
  self["port"] = SSN_PORT;

  mesh[0] = self;

  uint16_t i = 0;
  while(true) {
    String hostname = MDNS.hostname(i);

    if(hostname == "") break;

    JSONVar ssn;

    ssn["hostname"] = hostname;
    ssn["id"] = MDNS.IP(i).toString();
    ssn["port"] = MDNS.port(i);

    mesh[i + 1] = ssn;
    i++;
  }

  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);
  this->send(200, APPLICATION_JSON, JSON.stringify(mesh));
}

void APIServer::handleWildcard() {
  Serial.println("Serving *");
  File f = this->files.open("/static/index.html.gz", "r");

  if(!f) {
    this->send(500, APPLICATION_JSON, INTERNAL_SERVER_ERROR);
  }

  this->streamFile(f, TEXT_HTML);
  f.close();
}

void APIServer::restoreWiFiConfig() {
  Serial.println("Reading WiFi configuration...");
  if(!this->files.exists(WIFI_CONFIG_FILE)) {
    Serial.println("Config unavailable.");
  } else {
    File f = this->files.open(WIFI_CONFIG_FILE, "r");

    String ssid = f.readStringUntil('\n');
    String pass = f.readStringUntil('\n');

    connect(ssid, pass);

    f.close();
  }
}

void APIServer::begin() {
  WebServer::begin();

  this->restoreWiFiConfig();

  this->on("/api/login" , HTTP_OPTIONS, [this]() { this->handleOptions(); });
  this->on("/api/login",  HTTP_GET,     [this]() { this->handleApiLogin(); });
  this->on("/api/config",               [this]() { this->handleApiConfig(); });
  this->on("/api/mesh",   HTTP_GET,     [this]() { this->handleApiMesh(); });
  this->on("/api/data",                 [this]() { this->handleApiData(); });
  this->onNotFound(                     [this]() { this->handleWildcard(); });
  this->serveStatic("/static/",         this->files, "/static/", "max-age=86400");
}
