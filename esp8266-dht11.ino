#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include "Stats.h"

const int AVG_SAMPLES = 10;
const int SAMPLE_INTERVAL = 5000;
const int SENSOR = 2;
const int LED = 13;

const char* ssid = "SSID";
const char* password = "p455w0r0";

boolean error = true;
int errors = 0;
int measurements = 0;

Stats<float, AVG_SAMPLES> humidity(0.0f, 100.0f);
Stats<float, AVG_SAMPLES> temperature(-40.0f, 125.0f);

ESP8266WebServer server(80);
DHT dht(SENSOR, DHT11);

void handleRoot() {
  digitalWrite(LED, 1);
  Serial.println("Serving /");
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(LED, 0);
}

void handleTemp() {
  digitalWrite(LED, 1);
  Serial.println("Serving /temp");

  String message = "{";
  message += "\"humidity\":";
  message += String(humidity.last(), 2);
  message += ",\"avgHumidity\":";
  message += String(humidity.average(), 2);
  message += ",\"minHumidity\":";
  message += String(humidity.minimum(), 2);
  message += ",\"maxHumidity\":";
  message += String(humidity.maximum(), 2);
  message += ",\"temperature\":";
  message += String(temperature.last(), 2);
  message += ",\"avgTemperature\":";
  message += String(temperature.average(), 2);
  message += ",\"minTemperature\":";
  message += String(temperature.minimum(), 2);
  message += ",\"maxTemperature\":";
  message += String(temperature.maximum(), 2);
  message += ",\"errors\":";
  message += String(errors);
  message += ",\"measurements\":";
  message += String(measurements);
  message += ",\"sensor\":";
  message += error ? "\"error\"" : "\"ok\"";
  message += "}";

  server.send(200, "application/json", message);
  digitalWrite(LED, 0);
}

void handleNotFound() {
  digitalWrite(LED, 1);
  Serial.println("Serving 404");
  server.send(404, "text/plain", "File Not Found");
  digitalWrite(LED, 0);
}

void setup(void){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);

  Serial.begin(115200);
  Serial.println("");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/temp", handleTemp);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  dht.begin();
  readSensor();
  Serial.println("Sensor initialized");
}

void readSensor() {
  static uint32_t lastSampleTime = -SAMPLE_INTERVAL;
  uint32_t currTime = millis();

  if((currTime - lastSampleTime) < SAMPLE_INTERVAL) {
    return;
  }

  float hum = dht.readHumidity(true);
  float temp = dht.readTemperature(false, true);
  lastSampleTime = currTime;

  if(!isnan(hum) && !isnan(temp)) {
    humidity.add(hum);
    temperature.add(temp);
    measurements++;
    error = false;
  } else {
    errors++;
    error = true;
  }
}

void loop(void){
  readSensor();
  server.handleClient();
}
