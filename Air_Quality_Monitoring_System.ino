#define BLYNK_TEMPLATE_ID "*************"  //Your Blynk Template ID
#define BLYNK_TEMPLATE_NAME "Air Quality Monitor"  //Your Blynk Template Name
#define BLYNK_AUTH_TOKEN "*****************************"  //Your Blynk Authentication Token

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

byte degree_symbol[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "*********";  // Your Wi-Fi name
char pass[] = "*********";    // Your Wi-Fi password

BlynkTimer timer;

int gas = A0;
int sensorThreshold = 100;

#define DHTPIN 2  // Connect Out pin to D4 in NODE MCU
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int analogSensor = analogRead(gas);
  Blynk.virtualWrite(V2, analogSensor);
  Serial.print("Gas Value: ");
  Serial.println(analogSensor);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" Humidity: ");
  Serial.println(h);
}

void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(30000L, sendSensor);

  lcd.begin(16, 2);  // Initialize the LCD with 16 columns and 2 rows
  lcd.setBacklight(255); // Ensure backlight is on
  lcd.createChar(1, degree_symbol);  // Create the degree symbol

  lcd.setCursor(3, 0);
  lcd.print("Air Quality");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring");
  delay(4000);
  lcd.clear();
}

void loop() {
  Blynk.run();
  timer.run();

  float h = dht.readHumidity();
  float t = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit
  int gasValue = analogRead(gas);

  lcd.setCursor(0, 0);
  lcd.print("Temperature ");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(6, 1);
  lcd.write(1);  // Print degree symbol
  lcd.setCursor(7, 1);
  lcd.print("C");
  delay(4000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Humidity ");
  lcd.print(h);
  lcd.print("%");
  delay(4000);
  lcd.clear();

  if (gasValue < 600) {
    lcd.setCursor(0, 0);
    lcd.print("Gas Value: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Fresh Air");
    Serial.println("Fresh Air");
    delay(4000);
    lcd.clear();
  } else if (gasValue > 600) {
    lcd.setCursor(0, 0);
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Poor Air");
    Serial.println("Poor Air");
    delay(4000);
    lcd.clear();
  }

  if (gasValue > 600) {
    // Uncomment and configure email notifications if needed
    Blynk.email("youremail@gmail.com", "Alert", "Poor Air!");
    Blynk.logEvent("air_quality", "Poor Air");
  }
}
