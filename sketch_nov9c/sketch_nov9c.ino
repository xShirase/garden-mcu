#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Arduino JSON Library

// Wifi Credentials
const char* ssid = "G";               // Wifi SSID
const char* password = "Platypu5$y";  //Wi-FI Password

#define FERN_PIN 33
#define TEST_PIN 32




//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.86.191:3000";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);  // give me time to bring up serial monitor
  Serial.println("ESP32 Test");


  // // Connect to local WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print local IP address
  delay(2000);                     // wait for 2s
}

void loop() {

  int fern = analogRead(FERN_PIN);  // read the analog value from sensor
  Serial.print("Fern: ");
  Serial.println(fern);
  int value = analogRead(TEST_PIN);  // read the analog value from sensor
  Serial.print("TEST: ");
  Serial.println(value);

  delay(5000);


  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String serverPath = serverName + "/fern/" + fern;

    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());


    // Send HTTP GET request
    int httpResponseCode = http.GET();
    http.end();
    String s2 = serverName + "/test/" + value;
    http.begin(s2.c_str());
    int c2 = http.GET();

    if (httpResponseCode != 200) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}