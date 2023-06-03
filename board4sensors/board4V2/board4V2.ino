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
  showBrightness();
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

// Temperature

#define DHTPIN 18
DHT dht(DHTPIN, DHT22);

float dhtTemp;
float dhtHum;
float dhtHIC;

void startDHT() {
  dht.begin();
  Serial.println("Started DHT22");
}

void refreshDHT() {
  dhtHum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  dhtTemp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(dhtTemp) || isnan(dhtHum)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  dhtHIC = dht.computeHeatIndex(dhtHum, dhtTemp, false);
}

// OLED 

// U8X8_SSD1306_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int cursor=0;
int fontW=12;
int fontH=18;

void startDisplay() {
 display.begin();
 display.enableUTF8Print();
}

void prepareDisplay() {
  display.setFont(u8g2_font_helvR12_tf);
  display.setFontRefHeightExtendedText();
  display.setDrawColor(1);
  display.setFontPosTop();
  display.setFontDirection(0);
}

// Menus

#define MENU_SIZE 3
#define CONFIG_SIZE 4


const char* currentPage = "Main";

char *menu[MENU_SIZE] = { "Dashboard", "Lights", "Config"};
char *lightsMenu[LIGHTS_NUM] = { "Lights 1", "Lights 2", "Lights 3", "Lights 4" };
char *lightMenu[2] = { "Toggle", "Brightness" };
char *moistureMenu[MOISTURE_NUM] = { "Sensor 1", "Sensor 2", "Sensor 3", "Sensor 4" };

void showMenu(int size, char** m ) {
  cursor=0;
  display.clearDisplay();
  display.clearBuffer();
  prepareDisplay();

  if (size<=3){
    display.setFont(u8g2_font_helvR14_tf);
    fontW=10;
    fontH=14;
  } else {
    display.setFont(u8g2_font_6x10_tf);
    fontW=6;
    fontH=10;
  }

  for (int i = 0; i<size; i++) {
    display.setCursor(fontW+2,i*(fontH+2)+(2*i));
    display.print(m[i]);
  }
  display.setCursor(0,0);
  display.print('>');
  display.sendBuffer();
}

void showMainMenu() {
  currentPage="Main";
  showMenu(MENU_SIZE, menu);
}

void showLightsMenu() {
  cursor=0;
  currentPage="Lights";
  showMenu(LIGHTS_NUM, lightsMenu);
}

void showLightMenu(int i) {
  cursor=0;
  currentLight=i;
  currentPage="Light";
  showMenu(2, lightMenu);
}

void showConfigMenu() {
  cursor=0;
  currentPage="Config";
  showMenu(MOISTURE_NUM, moistureMenu);
}


void showDashboard() {
  cursor = 0;
  currentPage = "Dashboard";
  display.clearDisplay();
  display.setFont(u8g2_font_streamline_weather_t);
  display.setCursor(0, 0);
  display.print("\0036");
  display.setFont(u8g2_font_helvR12_tf);
  display.print(dhtTemp);
  display.print("°");
  display.setCursor(0, 23);
  display.setFont(u8g2_font_streamline_travel_wayfinding_t);
  display.print("\0033");
  display.setFont(u8g2_font_helvR12_tf);
  display.print(dhtHum);
  display.print("%");
  display.setCursor(88, 0);
  display.setFont(u8g2_font_streamline_ecology_t);
  display.print("\0039");
  display.setFont(u8g2_font_HelvetiPixel_tr);
  for (int i=0; i<MOISTURE_NUM; i++) {
    display.setCursor(88, 22+(10*i));
    display.print(moistureValues[i]);
  }
  display.sendBuffer();
  // showMainMenu();
}

void showBrightness() {
  cursor = 0;
  currentPage = "Brightness";
  display.clearDisplay();
  display.setFont(u8g2_font_helvR12_tf);
  display.setCursor(0, 0);
  display.print(lightLevels[currentLight]);
  display.sendBuffer();
  // showMainMenu();
}

void selectFromMain(String menu){
  Serial.println(menu);
  if(menu=="Dashboard"){
    showDashboard();
  } else if(menu=="Lights"){
    showLightsMenu();
  }
}

void selectFromMenu(){
  if(currentPage == "Main") selectFromMain(menu[cursor]);
  else if(currentPage == "Lights") selectFromLightsMenu(cursor);
  else if(currentPage == "Light") selectFromLightMenu(cursor);
  else if(currentPage == "Dashboard") showMainMenu();
  else if(currentPage == "Config") showMainMenu();
  else if (currentPage == "Brightness") showLightsMenu();
}

void selectFromLightsMenu(int i){
  showLightMenu(i);
}

void selectFromLightMenu(int i){
  if(i==0){
    switchLight(lightsPins[currentLight]);
    showMainMenu();
  }
  if(i==1){
    showBrightness();
  }
}

void navigateMenu(int i, int size){
  // erase previous cursor:
  display.setCursor(0, cursor*(fontH+2)+(2*cursor));
  display.setDrawColor(0);
  display.drawBox(0, cursor*(fontH+2)+(2*cursor), fontW+2, fontH+2);
  if (i==0) {
    cursor++;
    if (cursor>(size-1)) cursor=0;
  } else if(i==1) {
    cursor--;
    if (cursor<0) cursor=(size-1);
  }
  display.setCursor(0,cursor*(fontH+2)+(2*cursor));
  display.setDrawColor(1);
  display.print('>');
  display.sendBuffer();

}

// Buttons
byte button_pins[] = {17, 16, 5}; // button pins: up, down, select
#define NUMBUTTONS sizeof(button_pins)
Bounce * buttons = new Bounce[NUMBUTTONS];

#define BTNPIN 15

void startButtons(){
  for (int i=0; i<NUMBUTTONS; i++) {
    buttons[i].attach( button_pins[i], INPUT_PULLUP); // setup the bounce instance for the current button
    buttons[i].interval(25); // interval in ms
  }
}

void refreshButtons() {
  for (int i = 0; i<NUMBUTTONS; i++) {
    buttons[i].update();
    if ( buttons[i].fell() ) {
      if (i==2) { // select
         selectFromMenu();
      } else {
        if(currentPage == "Main") navigateMenu(i,MENU_SIZE);
        else if(currentPage == "Lights") navigateMenu(i,LIGHTS_NUM);
        else if(currentPage == "Light") navigateMenu(i,2);
        else if(currentPage == "Brightness") adjustLevel(i);
        else return;
      }
    }
  }
}

// Commands

void processCommand(String cmd) {
  Serial.print("Received ");
  Serial.println(cmd);
  if(cmd=="debug"){
    if(debug==true) debug = false;
    else debug = true;
  }
}

int start = millis();

void setup() {
  Serial.begin(115200);
  delay(3000);
  startWifi();
  startLights();
  startDHT();
  startMoisture();
  startDisplay();
  startButtons();
  showMainMenu();
}

void loop() {
  if(Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); 
    processCommand(cmd);
  }

  refreshButtons();
  int now = millis();
  if(now-start>3000){
    refreshDHT();
    refreshMoisture();
    if(currentPage == "Dashboard"){
      showDashboard();
    }
    start = now;
  }
}

