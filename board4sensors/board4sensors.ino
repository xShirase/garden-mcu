#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MCP9808.h"
#include <DHT.h>

// Wifi Credentials
const char* ssid = "G";               // Wifi SSID
const char* password = "Platypu5$y";  //Wi-FI Password
String serverName = "http://192.168.86.191:3000";

// GPIO definition

#define M0PIN 33
#define M1PIN 32
#define M2PIN 35
#define M3PIN 34
#define M0LED 13
#define M1LED 12
#define M2LED 4
#define M3LED 2
#define DHTPIN 19
#define MOSFET 26

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temp

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
DHT dht(DHTPIN, DHT22);

// Vars init

uint32_t delayMS;
int m0;
int m1;
int m2;
int m3;
int DRY = 1200;

int start = millis();


void refreshMoisture() {
  m0 = analogRead(M0PIN);
  m1 = analogRead(M1PIN);
  m2 = analogRead(M2PIN);
  m3 = analogRead(M3PIN);

  if(m0>DRY){digitalWrite(M0LED, HIGH);}else{digitalWrite(M0LED, LOW);}
  if(m1>DRY){digitalWrite(M1LED, HIGH);}else{digitalWrite(M1LED, LOW);}
  if(m2>DRY){digitalWrite(M2LED, HIGH);}else{digitalWrite(M2LED, LOW);}
  if(m3>DRY){digitalWrite(M3LED, HIGH);}else{digitalWrite(M3LED, LOW);}
  
  Serial.print(m0);
  Serial.print(", ");
  Serial.print(m1);
  Serial.print(", ");
  Serial.print(m2);
  Serial.print(", ");
  Serial.println(m3);
}

void refreshDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("M0: ");
  display.println(m0);
  display.print("M1: ");
  display.println(m1);
  display.print("M2: ");
  display.println(m2);
  display.print("M3: ");
  display.println(m3);
  display.display(); 
}

void refreshTemp() {
  tempsensor.wake();   // wake up, ready to read!
  float c = tempsensor.readTempC();
  Serial.print("Temp: "); 
  Serial.print(c, 4); 
  Serial.println("C"); 
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mA, stops temperature sampling
}

void refreshDHT() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C, Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C"));
}

void setPins() {
  pinMode(M0PIN, INPUT);
  pinMode(M1PIN, INPUT);
  pinMode(M2PIN, INPUT);
  pinMode(M3PIN, INPUT);
  pinMode(M0LED, OUTPUT);
  pinMode(M1LED, OUTPUT);
  pinMode(M2LED, OUTPUT);
  pinMode(M3LED, OUTPUT);
  pinMode(MOSFET, OUTPUT);
}

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

void startDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void startTemp() {
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
    
   Serial.println("Found MCP9808!");

  tempsensor.setResolution(1); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
}



void fadeLamp() {
  for(int i = 0; i<360; i++){
    //convert 0-360 angle to radian (needed for sin function)
    float rad = DEG_TO_RAD * i;

    //calculate sin of angle as number between 0 and 255
    int sinOut = constrain((sin(rad) * 128) + 128, 0, 255); 

    analogWrite(MOSFET, sinOut);

    delay(15);
  }
}


void setup() {
  Serial.begin(115200);
  delay(3000);
  setPins();
  connectWifi();
  Serial.println("Start");
  startDisplay();
  startTemp();
  startDHT();
  display.display();
  delay(2000);
}

void loop() {
  int now = millis();
  if(now-start>3000){
    refreshTemp();
    refreshDHT();
    refreshMoisture();
    refreshDisplay();
    fadeLamp();
    start = now;
  }
}
