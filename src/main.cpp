#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6effcxvfN"
#define BLYNK_TEMPLATE_NAME "AquariumMonitoringSystem"
#define BLYNK_AUTH_TOKEN "1gQNBkK05gkTa-ohVKlFMKjxZUM3_FbA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// ------------------- Blynk Config -------------------
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

BlynkTimer timer;

// ------------------- Pin Setup -------------------
#define ONE_WIRE_BUS 15
#define LDR_PIN 34
#define LED_TEMP 18
#define LED_LIGHT 19

// ------------------- Sensor & LCD Setup -------------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- Kirim & Tampilkan Data -------------------
void sendSensorData() {
  sensors.requestTemperatures();
  float suhu = sensors.getTempCByIndex(0);
  int ldrValue = analogRead(LDR_PIN);
  float intensitas = map(ldrValue, 0, 4095, 0, 100);

  // Kirim ke Blynk
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, intensitas);

  // Tampilkan ke LCD
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(suhu, 1);
  lcd.print((char)223);
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Cahaya: ");
  lcd.print(intensitas, 0);
  lcd.print("%    ");

  // Kontrol LED otomatis
  digitalWrite(LED_TEMP, suhu < 25.0 ? HIGH : LOW);
  digitalWrite(LED_LIGHT, intensitas < 30.0 ? HIGH : LOW);

  // ------------------ Notifikasi Terminal ------------------
  if (suhu < 25.0) {
    Serial.println("⚠️  Suhu terlalu rendah! Aktifkan pemanas.");
  }

  if (intensitas < 30.0) {
    Serial.println("⚠️  Intensitas cahaya rendah! Nyalakan lampu aquarium.");
  }

  // Tambahan informasi ke terminal
  Serial.print("Suhu: ");
  Serial.print(suhu, 1);
  Serial.print(" C | Intensitas: ");
  Serial.print(intensitas, 0);
  Serial.println(" %");
}

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  sensors.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  lcd.setCursor(1, 0);
  lcd.print("Smart Aquarium");
  delay(1000);
  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("IoT- Project");
  delay(1000);
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("Kelompok 7");
  delay(2000);
  lcd.clear();

  timer.setInterval(1000L, sendSensorData);
}

// ------------------- Loop -------------------
void loop() {
  Blynk.run();
  timer.run();
}
