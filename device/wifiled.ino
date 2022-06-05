#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char* ssid = "MikroTik-2G";
const char* password = "qwertyuiop";

#define DHTPIN 5
#define FANPIN 4
#define LEDPIN 0
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

float t = 0.0;
float h = 0.0;
int fan = 1;
int led = 1;

// Server at :80
AsyncWebServer server(80);

unsigned long previousMillis = 0;    
// Updates DHT readings every 10 seconds
const long interval = 10000;

void setup(){
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println(WiFi.localIP());

  pinMode(FANPIN, OUTPUT);
  digitalWrite(FANPIN, HIGH);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);

  // Status API
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    DynamicJsonDocument doc(96);
    String output;
    doc["status"] = 1;
    doc["temperature"] = t;
    doc["humidity"] = h;
    doc["fan"] = fan;
    doc["led"] = led;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
  });

  // Fan API
  server.on("/fanOn", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(FANPIN, HIGH);
    fan = 1;
    request->send(200, "text/plain", "FAN ON");
  });
  server.on("/fanOff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(FANPIN, LOW);
    fan = 0;
    request->send(200, "text/plain", "FAN OFF");
  });
  // Fan API
  server.on("/ledOn", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LEDPIN, HIGH);
    led = 1;
    request->send(200, "text/plain", "LED ON");
  });
  server.on("/ledOff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(LEDPIN, LOW);
    led = 0;
    request->send(200, "text/plain", "LED OFF");
  });

  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    float newT = dht.readTemperature();
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor");
    } else {
      t = newT;
      Serial.println(t);
    }

    float newH = dht.readHumidity();
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor");
    } else {
      h = newH;
      Serial.println(h);
    }

    if (t > 28.0) {
      digitalWrite(FANPIN, HIGH);
      fan = 1;
    } else if (t < 27.5) {
      digitalWrite(FANPIN, LOW);
      fan = 0;
    }
  }
}
