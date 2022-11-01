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
  bool result = waitForConnection(WIFI_CONNECTION_TIMEOUT);

  if(result) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  return result;
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
    Serial.println("Saving WiFi configuration...");
    File f = this->files.open(WIFI_CONFIG_FILE, "w");
    f.write((const uint8_t*)ssid.c_str(), ssid.length());
    f.write('\n');
    f.write((const uint8_t*)pass.c_str(), pass.length());
    f.write('\n');
    f.close();

    this->send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    this->send(403, "application/json", "{\"error\":\"Invalid credentials.\"}");
  }
}

void APIServer::handleApiData() {
  Serial.println("Serving /api/data");
  this->send(200, "application/json", this->device->toJSON());
}

void APIServer::handleApiMesh() {
  Serial.println("Serving /api/mesh");
  JSONVar mesh;

  uint16_t n = MDNS.queryService("ssn", "tcp");

  for (uint16_t i = 0; i < n; i++) {
    JSONVar ssn;

    ssn["hostname"] = MDNS.hostname(i);
    ssn["id"] = MDNS.IP(i);
    ssn["port"] = MDNS.port(i);

    mesh[i] = ssn;
  }

  this->send(200, "application/json", JSON.stringify(mesh));
}

void APIServer::handleWildcard() {
  Serial.println("Serving *");
  File f = this->files.open("/static/index.html.gz", "r");

  if(!f) {
    this->send(500, "application/json", "{\"error\":\"Internal server error.\"}");
  }

  this->streamFile(f, "text/html");
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

  // FIXME Some browsers send lowercase authorization header.
  const char *headers[] = { LOWER_CASE_AUTHORIZATION_HEADER };
  this->collectHeaders(headers, sizeof(headers)/sizeof(headers[0]));

  this->on("/api/login" , HTTP_OPTIONS, [this]() { this->handleOptions(); });
  this->on("/api/login",  HTTP_GET,     [this]() { this->handleApiLogin(); });
  this->on("/api/config",               [this]() { this->handleApiConfig(); });
  this->on("/api/mesh",   HTTP_GET,     [this]() { this->handleApiMesh(); });
  this->on("/api/data",                 [this]() { this->handleApiData(); });
  this->onNotFound(                     [this]() { this->handleWildcard(); });
  this->serveStatic("/static/",         this->files, "/static/", "max-age=86400");
}
