#define BLYNK_TEMPLATE_ID "xxxxxxxxxxxxxxxxxxxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "Water monitoring system"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxxxxxxxxxxxxx"

#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// ======= Wi-Fi =======
char ssid[] = "Ghost";
char pass[] = "111222333";

// ======= Pins =======
LiquidCrystal_I2C lcd(0x27, 20, 4); // change to 0x3F if needed
Adafruit_BMP280 bmp;
bool bmpOK = false;

const int BUZZER_PIN = 26;
const int SOIL_PIN   = 36;
const int RAIN_PIN   = 35;
const int MQ_PIN     = 32;
const int DUST_PIN   = 33;

// ======= Blynk Timer =======
BlynkTimer timer;

// ======= Buzzer variables =======
unsigned long buzzerStartTime = 0;
bool buzzerOn = false;

// ======= I2C scan =======
void scanI2C() {
  Serial.println("I2C scan:");
  int nDevices = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print(" I2C device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println(" No I2C devices found");
  else {
    Serial.print(" Devices found: ");
    Serial.println(nDevices);
  }
}

// ======= Buzzer =======
void checkBuzzer(int gasValue, int rainValue) {
  if ((gasValue > 2500 || rainValue == LOW) && !buzzerOn) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerOn = true;
    buzzerStartTime = millis();
  }
  if (buzzerOn && (millis() - buzzerStartTime >= 1000)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }
}

// ======= Read Sensors and Update LCD =======
void readAndPublish() {
  int soilValue = analogRead(SOIL_PIN);
  int rainValue = digitalRead(RAIN_PIN); // LOW = rain
  int gasValue  = analogRead(MQ_PIN);
  int dustADC   = analogRead(DUST_PIN);

  float dustVoltage = (dustADC / 4095.0f) * 3.3f;
  float dustDensity = (dustVoltage - 0.6f) * 500.0f;
  if (dustDensity < 0) dustDensity = 0;

  float tempC = 0.0f;
  float pres_hPa = 0.0f;
  if (bmpOK) {
    tempC = bmp.readTemperature();
    pres_hPa = bmp.readPressure() / 100.0f;
  }

  checkBuzzer(gasValue, rainValue);

  lcd.clear();
  delay(5);

  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(soilValue);
  lcd.print(" R:");
  lcd.print((rainValue==LOW)?1:0);
  lcd.print(" G:");
  lcd.print(gasValue);

  lcd.setCursor(0, 1);
  lcd.print("D:");
  lcd.print((int)dustDensity);
  lcd.print(" T:");
  lcd.print(tempC, 1);
  lcd.print(" P:");
  lcd.print((int)pres_hPa);

  lcd.setCursor(0,2);
  lcd.print("                    "); // clear line
  lcd.setCursor(0,3);
  lcd.print("                    "); // clear line

  Serial.print("Soil:"); Serial.print(soilValue);
  Serial.print(" Rain:"); Serial.print((rainValue==LOW)?1:0);
  Serial.print(" Gas:"); Serial.print(gasValue);
  Serial.print(" Dust:"); Serial.print((int)dustDensity);
  Serial.print(" Temp:"); Serial.print(tempC,1);
  Serial.print("C Press:"); Serial.println(pres_hPa,1);

  Blynk.virtualWrite(V0, soilValue);
  Blynk.virtualWrite(V1, (rainValue==LOW)?1:0);
  Blynk.virtualWrite(V2, gasValue);
  Blynk.virtualWrite(V3, dustDensity);
  Blynk.virtualWrite(V4, tempC);
  Blynk.virtualWrite(V5, pres_hPa);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("=== Weather Monitoring System (BMP280) ===");

  Wire.begin(21,22);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(RAIN_PIN, INPUT_PULLUP);

  lcd.init();
  delay(50);
  lcd.backlight();
  lcd.clear();
  delay(10);
  lcd.setCursor(0,0);
  lcd.print("Starting...");

  scanI2C();

  bmpOK = false;
  if (bmp.begin(0x76) || bmp.begin(0x77)) bmpOK = true;

  lcd.setCursor(0,1);
  if (bmpOK) lcd.print("BMP280 OK     ");
  else lcd.print("BMP NOT FOUND ");

  // WiFi connect (LCD-এ দেখাবো না)
  WiFi.begin(ssid, pass);
  int wifiTry = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTry < 30) {
    delay(500);
    Serial.print(".");
    wifiTry++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) Serial.println("WiFi connected!");
  else Serial.println("WiFi failed");

  // Timer
  timer.setInterval(2000L, readAndPublish);

  // সেন্সর মান দেখাও আগে
  delay(200);
  readAndPublish();
  delay(200);

  // Blynk connect (LCD-এ দেখাবো না)
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk init done");
}

void loop() {
  Blynk.run();
  timer.run();
}
