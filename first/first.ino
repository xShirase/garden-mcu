#define LED_BUILTIN 2
#define LED_PIN 4
int touch_value = 100;
#define AOUT_PIN 15

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  delay(2000); // give me time to bring up serial monitor
  Serial.println("ESP32 Test");
}

void loop() {

  int value = analogRead(AOUT_PIN); // read the analog value from sensor

  Serial.print("Moisture value: ");
  Serial.println(value);

  touch_value = touchRead(TOUCH_PIN);


  delay(1000);
}
