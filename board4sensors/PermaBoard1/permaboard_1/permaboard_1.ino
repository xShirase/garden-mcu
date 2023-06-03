// 4 LED on mosfets
// 4 Moisture sensors
// 12V VCC
// 8-12h timer
// API
// Sensor events
// Retain config

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Bounce2.h>

bool debug = false;

// Wifi
const char* ssid = "G";               // Wifi SSID
const char* password = "Platypu5$y";  //Wi-FI Password
String serverName = "http://192.168.86.191:3000";

void startWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Started Wifi");
}

// Peripherals definitions

// Lights

#define LIGHTS_NUM 4
#define Q1PIN 27
#define Q2PIN 14
#define Q3PIN 12
#define Q4PIN 13
int lightsPins[4] = { Q1PIN, Q2PIN, Q3PIN, Q4PIN };
int lightLevels[4] = { 130, 130, 130, 130 };

int currentLight;

void startLights() {
  for (int i=0; i<LIGHTS_NUM; i++) {
    pinMode(lightsPins[i], OUTPUT);
  }
  Serial.println("Started Lights");
}

void switchLight(int pin){
  if(lightLevels[currentLight]==0){
    analogWrite(pin, lightLevels[currentLight]);
  }else {
    digitalWrite(pin, LOW);
  }
}

void adjustLevel(int direction){
  if(direction == 0) lightLevels[currentLight] += 10;
  if(direction == 1) lightLevels[currentLight] -= 10;
  if(lightLevels[currentLight] > 255) lightLevels[currentLight] = 0;
  if(lightLevels[currentLight] < 0 ) lightLevels[currentLight] = 255;
  Serial.println(lightLevels[currentLight]);
  analogWrite(lightsPins[currentLight], lightLevels[currentLight]);
}

// Moisture

#define M0PIN 33
#define M1PIN 32
#define M2PIN 35
#define M3PIN 34
#define MOISTURE_NUM 4

int moisturePins[] = { M0PIN, M1PIN, M2PIN, M3PIN };
int moistureValues[] = { 0,0,0,0 };
int DRY_TRESH = 1300;

void startMoisture() {
  for (int i=0; i<MOISTURE_NUM; i++) {
    pinMode(moisturePins[i], INPUT);
  }
  Serial.println("Started Moisture");
}

void refreshMoisture() {
  for (int i=0; i<MOISTURE_NUM; i++) {
    moistureValues[i] = analogRead(moisturePins[i]);
  }
}

void printMoisture(){
  Serial.println("dis");
  for (int i=0; i<MOISTURE_NUM; i++) {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(moistureValues[i]);
  }
}
// Commands

void processCommand(String cmd, String id) {
  if(cmd=="l") {
    int light = id.toInt() - 1 ;
    if(light>=0 && light<LIGHTS_NUM) switchLight(lightsPins[light]);
  }
  if(cmd=="d") printMoisture();

}

int start = millis();

void setup() {
  Serial.begin(115200);
  delay(3000);
  startWifi();
  startLights();
  startMoisture();
}

void loop() {
  if(Serial.available()) {
    String cmd = Serial.readStringUntil(' ');
    String id = Serial.readStringUntil('\n'); 
    processCommand(cmd, id);
  }

  int now = millis();
  if(now-start>3000){
    refreshMoisture();
    start = now;
  }
}



