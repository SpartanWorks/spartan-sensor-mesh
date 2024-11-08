#ifndef __APISERVER_HPP__
#define __APISERVER_HPP__

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define WebServer ESP8266WebServer

#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#endif

#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <FS.h>
#include "System.hpp"

#define SSM_PORT 80

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
  System &system;
  FS &files;

  void handleOptions();
  void handleApiLogin();
  void handleApiConfig();
  void handleApiReset();
  void handleApiMesh();
  void handleApiData();
  void handleWildcard();

  void restoreWiFiConfig();
  void sendJSON(int code, JSONVar& json);

public:
  APIServer(System &s, FS &fs);
  void begin();
};

#endif
