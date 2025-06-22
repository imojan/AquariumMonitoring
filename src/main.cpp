#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6effcxvfN"
#define BLYNK_TEMPLATE_NAME "AquariumMonitoringSystem"
#define BLYNK_AUTH_TOKEN "1gQNBkK05gkTa-ohVKlFMKjxZUM3_FbA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ------------------- WiFi Config -------------------
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "sare";           // Ganti dengan nama WiFi kamu
char pass[] = "kepoluuu";       // Ganti dengan password WiFi kamu

BlynkTimer timer;

// ------------------- Pin Setup -------------------
#define ONE_WIRE_BUS 5
#define LDR_PIN 32          // D0 LDR disambung ke D4 ESP32 (GPIO2)
#define LED_TEMP 18
#define LED_LIGHT 19

// ------------------- Sensor & LCD Setup -------------------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address bisa 0x27 atau 0x3F

void scanWiFiNetworks() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning WiFi...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(1000);

  int n = WiFi.scanNetworks();
  Serial.println("Hasil Scan WiFi:");
  if (n == 0) {
    Serial.println("Tidak ada jaringan ditemukan.");
  } else {
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(" dBm)");
    }
  }
  delay(2000);
  lcd.clear();
}

// ------------------- Kirim & Tampilkan Data -------------------
void sendSensorData() {
  sensors.requestTemperatures();
  float suhu = sensors.getTempCByIndex(0);
  int ldrValue = analogRead(LDR_PIN);
  float intensitas = ((4095 - ldrValue) / 4095.0) * 100.0;

  Serial.print("Raw LDR Value (analog): ");
  Serial.println(ldrValue);

  // Kirim ke Blynk
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V2, intensitas);

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
  digitalWrite(LED_LIGHT, intensitas < 25.0 ? HIGH : LOW);

  // Terminal log
  if (suhu < 25.0) {
    Serial.println("⚠️  Suhu terlalu rendah! Aktifkan pemanas.");
  }
  if (intensitas < 25.0) {
    Serial.println("⚠️  Intensitas cahaya rendah! Nyalakan lampu aquarium.");
  }

  Serial.print("Suhu: ");
  Serial.print(suhu, 1);
  Serial.print(" C | Intensitas: ");
  Serial.print(intensitas, 0);
  Serial.println(" %");
}


// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA = 21, SCL = 22

  lcd.init();
  lcd.backlight();

  scanWiFiNetworks();

  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass);

  sensors.begin();
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Smart Aquarium");
  delay(4000);
  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("IoT- Project");
  lcd.setCursor(2, 1);
  lcd.print("Project -UAS");
  delay(3000);
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("Kelompok 7");
  lcd.setCursor(3, 1);
  lcd.print("Kelas -T4J");
  delay(2500);
  lcd.clear();

  timer.setInterval(1000L, sendSensorData);
}

// ------------------- Loop -------------------
void loop() {
  Blynk.run();
  timer.run();
}
