#ifndef __APISERVER_HPP__
#define __APISERVER_HPP__

#include <Arduino_JSON.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <FS.h>
#include <ESPmDNS.h>
#include "Device.hpp"

#define SSN_PORT 80

#define WIFI_CONNECTION_TIMEOUT 20000 // 20 seconds

#define WIFI_CONFIG_FILE "/wifi_config.txt"

#define APPLICATION_JSON "application/json"
#define TEXT_HTML "text/html"

#define STATUS_OK "{\"status\":\"ok}"
#define UNAUTHORIZED "{\"error\":\"Unauthorized.\"}"
#define INVALID_CREDENTIALS "{\"error\":\"Invalid credentials.\"}"
#define INTERNAL_SERVER_ERROR "{\"error\":\"Internal server error.\"}"

#define LOWER_CASE_AUTHORIZATION_HEADER "authorization"
#define AUTHORIZATION_HEADER "Authorization"
#define ALLOWED_HEADER "Access-Control-Allow-Headers"
#define CORS_HEADER "Access-Control-Allow-Origin"

#define ALLOWED_ORIGIN "*"

class APIServer: public WebServer {
private:
  const Device *device = nullptr;
  FS &files;

  void handleOptions();
  void handleApiLogin();
  void handleApiConfig();
  void handleApiMesh();
  void handleApiData();
  void handleWildcard();

  void restoreWiFiConfig();

public:
  APIServer(const Device *d, FS &fs);
  void begin();
};

#endif
