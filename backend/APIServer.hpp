#ifndef __APISERVER_HPP__
#define __APISERVER_HPP__

#include <Arduino_JSON.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <FS.h>
#include <ESPmDNS.h>
#include "Device.hpp"

const int WIFI_CONNECTION_TIMEOUT = 20000; // 20 seconds
const String WIFI_CONFIG_FILE = "/wifi_config.txt";

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
  APIServer(uint16_t port, const Device *d, FS &fs);
  void begin();
};

#endif
