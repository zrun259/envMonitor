#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_AGS02MA.h"
#include <Wire.h>

#define DHT_DATA_PIN              18
#define LIGHT1_ADC_PIN            34
#define LIGHT2_ADC_PIN            39
#define PIR_DATA_PIN              35
#define SOFT_I2C_SDA              17
#define SOFT_I2C_SCL              16
// put function declarations here:
int humidity;
float temperature;
int light1_adc_value;
int light2_adc_value;
uint32_t tvoc;
sensors_event_t a, g, temp;

int myFunction(int, int);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);
Adafruit_MPU6050 mpu;
DHT dht(DHT_DATA_PIN,DHT11);
Adafruit_AGS02MA ags;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire1.begin(SOFT_I2C_SDA, SOFT_I2C_SCL, 400000);
  pinMode(LIGHT1_ADC_PIN, INPUT);
  pinMode(LIGHT2_ADC_PIN, INPUT);
  pinMode(PIR_DATA_PIN, INPUT);
  if (! ags.begin(&Wire1, 0x1A)) {
  //if (! ags.begin(&Wire1, 0x1A)) { // or use Wire1 instead!
    Serial.println("Couldn't find AGS20MA sensor, check your wiring and pullup resistors!");
    while (1) yield();
  }

  if (ags.getFirmwareVersion() == 0) {
    Serial.println(F("Could not read firmware, I2C communications issue?"));
    while (1) yield();
  }

  Serial.print("Firmware version: 0x");
  Serial.println(ags.getFirmwareVersion(), HEX);
  ags.printSensorDetails();
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  if (!mpu.begin(0x68, &Wire1)) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");
  display.display();
  delay(500); // Pause for 0.5 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  humidity = dht.readHumidity();
  tvoc = ags.getTVOC();
  temperature = dht.readTemperature();
  light1_adc_value = analogRead(LIGHT1_ADC_PIN);
  light2_adc_value = analogRead(LIGHT2_ADC_PIN);
  int pir_value = digitalRead(PIR_DATA_PIN);
  mpu.getEvent(&a, &g, &temp);
  Serial.println("Hello, World!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("acc: ");
  display.print(a.acceleration.x, 1);
  display.print(", ");
  display.print(a.acceleration.y, 1);
  display.print(", ");
  display.print(a.acceleration.z, 1);
  display.println("");
  display.print("gyo: ");
  display.print(g.gyro.x, 1);
  display.print(", ");
  display.print(g.gyro.y, 1);
  display.print(", ");
  display.print(g.gyro.z, 1);
  display.println("");
  display.print("H:");display.print(humidity);display.print(" ");
  display.print("T:");display.print(temperature);display.println("");
  display.print("Light:");display.print(light1_adc_value);display.print(" ");display.print(light2_adc_value);display.println("");
  display.print("PIR:");display.print(pir_value);display.println("");
  display.print("TVOC:");display.print(tvoc);display.println("");
  display.display();
  delay(100);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}