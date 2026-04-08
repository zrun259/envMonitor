#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include "Sensor.hpp"
#include "PirSensor.hpp"
#include "MpuSensor.hpp"
#include "LightSensor.hpp"
#include "DhtSensor.hpp"
#include "AgsSensor.hpp"

#define SOFT_I2C_SDA              17
#define SOFT_I2C_SCL              16
#define WIFI_SSID                  "Mi3000T_2.4G"
#define WIFI_PASSWORD              "123mi123"

DataTable_t* dataTable;

DhtSensor dht(dataTable,18,DHT11,4000);


Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire1.begin(SOFT_I2C_SDA, SOFT_I2C_SCL, 400000);
  Serial.println("Found a MPU-6050 sensor");
  dht.init();
  display.clearDisplay();
  display.display();
  delay(500); // Pause for 0.5 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  display.clearDisplay();
  display.setCursor(0, 0); 
  display.print("H:");display.print(dataTable->humidity);display.print(" ");
  display.display();
  delay(3000);
}