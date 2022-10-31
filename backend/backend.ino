#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "APIServer.hpp"
#include "BMPHub.hpp"
#include "DallasTempHub.hpp"
#include "Device.hpp"
#include "DHTHub.hpp"
#include "HTUHub.hpp"
#include "SDSHub.hpp"
#include "MHZHub.hpp"
#include "CCSHub.hpp"
#include "GP2YHub.hpp"
#include "System.hpp"

const int HTTP_PORT = 80;
const int AP_TIMEOUT = 900000; // 15 minutes
const int TIME_SLICE = 500; // 500 us
const String PASSWORD = "53n50rp455w0r0"; // WiFi AP & Device config password.

const int SDA_PIN = 21;
const int SCL_PIN = 22;

const int MHZ_RX = 13;
const int MHZ_TX = 12;

const int GP2Y_RX = 14;
const int GP2Y_TX = 15;

System ssn(PASSWORD, TIME_SLICE);

void setup(void){
  // SYSTEM
  ssn.begin();
  Serial.println("System initialized");

  // COMMUNICATION BUSSES
  HardwareSerial& sdsSerial(Serial2);

  // DEVICE TREE
  BMPHub *bmp = new BMPHub(&Wire, 0x76);
  bmp->begin(ssn);

  HTUHub *htu = new HTUHub(&Wire, 0x40);
  htu->begin(ssn);

  SDSHub *sds = new SDSHub(sdsSerial);
  sds->begin(ssn);

  MHZHub *mhz = new MHZHub(MHZ_RX, MHZ_TX);
  mhz->begin(ssn);

  CCSHub *ccs = new CCSHub(&Wire, 0x5A);
  ccs->begin(ssn);
  htu->compensate(ccs);

  GP2YHub *gp2y = new GP2YHub(GP2Y_RX, GP2Y_TX);
  gp2y->begin(ssn);

  // Other available sensors:
  // DallasTempHub *dallas = new DallasTempHub(2, 12);
  // dallas->begin(ssn);

  // DHTHub *dht11 = new DHTHub(2, DHT11);
  // dht11->begin(ssn);

  // DHTHub *dht22 = new DHTHub(2, DHT22);
  // dht22->begin(ssn);

  Wire.begin(SDA_PIN, SCL_PIN); // FIXME Needed as BMP280 library seems to override it.

  Serial.println("Device tree initialized");

  // NETWORK
  WiFi.hostname(ssn.device().name().c_str());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssn.device().name().c_str(), ssn.device().password().c_str());
  ssn.scheduler().spawn("disable AP", 125, [](Task *t) {
    static boolean apEnabled = true;

    if (apEnabled) {
      apEnabled = false;
      t->sleep(AP_TIMEOUT);
    } else {
      Serial.println("Disabling access point.");
      WiFi.mode(WIFI_STA);
      t->kill();
    }
  });

  Serial.println("WiFi initialized:");
  Serial.print("- SSID: ");
  Serial.println(ssn.device().name());
  Serial.print("- Password: ");
  Serial.println(ssn.device().password());
  Serial.print("- IP address: ");
  Serial.println(WiFi.softAPIP());

  // SERVICE DISCOVERY
  MDNS.begin(ssn.device().name().c_str());
  Serial.println("mDNS responder initialized");

  // API
  APIServer *server = new APIServer(HTTP_PORT, &ssn.device(), SPIFFS);
  server->begin();
  ssn.scheduler().spawn("handle API", 110, [server](Task *t) {
    server->handleClient();
  });
  MDNS.addService("http", "tcp", HTTP_PORT);
  Serial.println("API server initialized");
}

void loop(void) {
  ssn.run();
}
