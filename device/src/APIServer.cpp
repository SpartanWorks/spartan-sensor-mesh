#include "APIServer.hpp"

APIServer::APIServer(const Device &d, Log &l, FS &fs): WebServer(SSM_PORT), device(d), log(l), files(fs) {
}

bool waitForConnection(Log &log, uint32_t timeout) {
  uint32_t i = 0;
  while((WiFi.status() != WL_CONNECTED) && (i < timeout)) {
    log.print(".");
    delay(500);
    i += 500;
  }
  log.println("");

  if(WiFi.status() != WL_CONNECTED) {
    log.warn("Connection failed.");
    return false;
  }

  return true;
}

bool connect(Log &log, const String ssid, const String password) {
  log.info("Connecting to %s", ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  bool result = waitForConnection(log, WIFI_CONNECTION_TIMEOUT);

  if(result) {
    log.info("Connected to %s", ssid.c_str());
    log.info("IP address: %s", WiFi.localIP().toString().c_str());
  }
  return result;
}

void APIServer::handleOptions() {
  log.debug("Serving OPTIONS");
  this->sendHeader(ALLOWED_HEADER, AUTHORIZATION_HEADER);
  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);
  this->send(200, APPLICATION_JSON, STATUS_OK);
}

void APIServer::handleApiLogin() {
  log.debug("Serving /api/login");

  bool authorized = this->authenticate(this->device.name().c_str(), this->device.password().c_str());

  return authorized ? this->send(200, APPLICATION_JSON, STATUS_OK) : this->requestAuthentication();
}

void APIServer::handleApiConfig() {
  log.debug("Serving /api/config");

  if(!this->authenticate(this->device.name().c_str(), this->device.password().c_str())) {
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

  if(connect(log, ssid, pass)) {
    log.debug("Saving WiFi configuration...");
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
  log.debug("Serving /api/data");
  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);

  JSONVar body = this->device.toJSONVar();
  this->sendJSON(200, body);
}

void APIServer::handleApiMesh() {
  log.debug("Serving /api/mesh");
  JSONVar mesh;

  JSONVar self;

  self["hostname"] = this->device.name();
  self["ip"] = WiFi.localIP().toString();
  self["port"] = SSM_PORT;

  mesh[0] = self;

  uint16_t i = 0;
  while(true) {
    String hostname = MDNS.hostname(i); // FIXME Causes an error log on the serial.

    if(hostname == "") break;

    JSONVar ssn;

    ssn["hostname"] = hostname;
    ssn["ip"] = MDNS.IP(i).toString();
    ssn["port"] = MDNS.port(i);

    mesh[i + 1] = ssn;
    i++;
  }

  this->sendHeader(CORS_HEADER, ALLOWED_ORIGIN);
  this->sendJSON(200, mesh);
}

void APIServer::handleWildcard() {
  log.debug("Serving *");
  File f = this->files.open("/static/index.html.gz", "r");

  if(!f) {
    this->send(500, APPLICATION_JSON, INTERNAL_SERVER_ERROR);
  }

  this->streamFile(f, TEXT_HTML);
  f.close();
}

void APIServer::restoreWiFiConfig() {
  log.debug("Reading WiFi configuration...");
  if(!this->files.exists(WIFI_CONFIG_FILE)) {
    log.debug("Config unavailable.");
  } else {
    File f = this->files.open(WIFI_CONFIG_FILE, "r");

    String ssid = f.readStringUntil('\n');
    String pass = f.readStringUntil('\n');

    connect(log, ssid, pass);

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

#ifdef ESP8266
size_t JSONsize(JSONVar& json) {
  String type = JSON.typeof_(json);

  if (type == "object") {
    JSONVar keys = json.keys();
    int len = keys.length();

    size_t size = 0;

    for (uint16_t i = 0; i < len; i++) {
      JSONVar key = keys[i];
      size += JSON.stringify(key).length() + 1; // key name, quotes and collon.
      JSONVar value = json[key];
      size += JSONsize(value);
    }

    return size + 2 + max(0, len - 1); // {} plus len-1 comas between properties.
  } else if (type == "array") {
    int len = json.length();

    size_t size = 0;
    for (uint16_t i = 0; i < len; i++) {
      JSONVar value = json[i];
      size += JSONsize(value);
    }

    return size + 2 + max(0, len - 1); // [] plus len-1 comas between items.
  } else {
    return JSON.stringify(json).length();
  }
}

void streamJSON(WiFiClient& client, JSONVar& json) {
  String type = JSON.typeof(json);

  if (type == "object") {
    client.write("{");

    JSONVar keys = json.keys();
    int len = keys.length();

    for (uint16_t i = 0; i < len; i++) {
      JSONVar key = keys[i];

      String head = JSON.stringify(key) + ":";
      client.write(head.c_str());
      JSONVar value = json[key];
      streamJSON(client, value);

      if (i < len - 1) {
        client.write(",");
      }
    }

    client.write("}");
  } else if (type == "array") {
    client.write("[");

    int len = json.length();
    for (uint16_t i = 0; i < len; i++) {
      JSONVar value = json[i];
      streamJSON(client, value);

      if (i < len - 1) {
        client.write(",");
      }
    }

    client.write("]");
  } else {
    client.write(JSON.stringify(json).c_str());
  }

  client.flush();
}

void APIServer::sendJSON(int code, JSONVar& json) {
  size_t size = JSONsize(json);
  this->setContentLength(size);
  this->send(200, APPLICATION_JSON, "");
  streamJSON(this->client(), json);
}
#endif

#ifdef ESP32
void APIServer::sendJSON(int code, JSONVar& json) {
  this->send(200, APPLICATION_JSON, JSON.stringify(json));
}
#endif
