# ESP32-Weather-Monitoring-System

This project is a smart water and environmental monitoring system using an ESP32. It reads multiple environmental sensors, displays the data on an I2C LCD, triggers a buzzer for alerts, and sends real-time telemetry to the Blynk IoT platform.

#1. Hardware Components

ESP32 – Main microcontroller that reads sensors, controls the buzzer, and sends data to Blynk.

BMP280 Sensor – Measures temperature and atmospheric pressure via I2C (SDA=21, SCL=22).

Soil Moisture Sensor – Analog input on GPIO36. Detects soil wetness.

Rain Sensor – Digital input on GPIO35. LOW indicates rain.

MQ Gas Sensor – Analog input on GPIO32. Detects gas concentration.

Dust Sensor – Analog input on GPIO33. Outputs voltage proportional to particulate density.

Buzzer – Connected to GPIO26. Sounds alerts if gas is high or rain detected.

I2C LCD (20x4) – Displays live sensor readings.

#2. Software Logic
WiFi & Blynk

Connects to WiFi using stored SSID and password.

Connects to Blynk platform using authentication token.

Sensor values are sent to Blynk virtual pins:

V0: Soil

V1: Rain

V2: Gas

V3: Dust density

V4: Temperature

V5: Pressure

Sensor Reading

Soil moisture, gas, and dust sensors are read as analog values.

Rain sensor is digital; LOW indicates water presence.

Dust voltage is converted to density using the formula:
dustDensity = (dustVoltage - 0.6) * 500

BMP280

Reads temperature in °C and pressure in hPa.

I2C addresses checked: 0x76 or 0x77.

LCD Display

20x4 I2C LCD shows live readings:

S:<soil> R:<rain> G:<gas>
D:<dust> T:<temp> P:<pressure>

Buzzer Control

Triggered if:

Gas exceeds 2500 (analog threshold) OR

Rain sensor reads LOW

Buzzer automatically turns off after 1 second.

Timers

BlynkTimer calls readAndPublish() every 2 seconds for updating sensors, LCD, and Blynk.

#3. Pin Summary
Component	ESP32 Pin	Type
Soil Sensor	36	Analog

Rain Sensor	35	Digital

MQ Gas Sensor	32	Analog

Dust Sensor	33	Analog

Buzzer	26	Digital

BMP280 SDA	21	I2C

BMP280 SCL	22	I2C

I2C LCD SDA	21	I2C

I2C LCD SCL	22	I2C

#4. Features

Real-time monitoring of soil, rain, gas, dust, temperature, and pressure.

Alerts for high gas levels or rainfall via buzzer.

Telemetry sent to Blynk for remote monitoring.

LCD display provides immediate local feedback.
