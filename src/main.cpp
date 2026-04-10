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

#define DHT_PIN           18
#define LIGHT_PIN_1       39
#define LIGHT_PIN_2       34
#define PIR_PIN           35
#define SOFT_I2C_SDA              17
#define SOFT_I2C_SCL              16
#define WIFI_SSID                  "Mi3000T_2.4G"
#define WIFI_PASSWORD              "123mi123"

DataTable_t datas;
DataTable_t* dataTable_p = &datas;

DhtSensor dht(dataTable_p,18,DHT11,4000);
LightSensor light(dataTable_p,LIGHT_PIN_1,LIGHT_PIN_2,1000);
PirSensor pir(dataTable_p,35,500);
AgsSensor ags(dataTable_p,0x1A,&Wire1,4000);
MpuSensor mpu(dataTable_p,&Wire1,0x68,100);


Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Wire1.begin(SOFT_I2C_SDA, SOFT_I2C_SCL,400000);
  
  dht.init();
  light.init();
  pir.init();
  mpu.init();
  mpu.start();
  dht.start();
  light.start();
  pir.start();
  ags.init();
  ags.start();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
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
  display.print("Humd:");display.print(datas.humidity);display.print(" Temp:");display.print(datas.temperature);display.println("");
  display.print("Acc:");display.print(datas.mpu6050.ax);display.print(" ");display.print(datas.mpu6050.ay);display.print(" ");display.print(datas.mpu6050.az);display.println("");
  display.print("Gyr:");display.print(datas.mpu6050.gx);display.print(" ");display.print(datas.mpu6050.gy);display.print(" ");display.print(datas.mpu6050.gz);display.println("");
  display.print("Light:");display.print(datas.light1_adc_value);display.print(" ");display.print(datas.light2_adc_value);display.println("");
  display.print("UGM3:");display.print(datas.ags02_value);display.println("");
  display.print("Pir:");display.print(datas.pir);display.println("");
  display.display();
  delay(50);
}