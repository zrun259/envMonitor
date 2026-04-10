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

void updateUI();

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
  updateUI();
  delay(10);
}

void updateUI() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // --- 顶部状态栏：环境数据 ---
    display.setCursor(0, 2);
    display.print("H:"); display.print((int)datas.humidity); display.print("%");
    display.setCursor(35, 2);
    display.print("T:"); display.print((int)datas.temperature); display.print("C");
    display.setCursor(70, 2);
    display.print("PIR:"); 
    if(datas.pir) {
        display.fillRect(95, 0, 28, 10, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.print(" ACT "); // 有人时反显
    } else {
        display.print(" --- ");
    }
    
    display.setTextColor(SSD1306_WHITE);

    // 分割线
    display.drawFastHLine(0, 12, 128, SSD1306_WHITE);

    // --- 中部：Acc 加速度数据 ---
    display.setCursor(0, 16);
    display.print("A"); // 缩短标签，腾出空间

    // X 轴：从 15 像素开始
    display.setCursor(15, 16);
    display.print("X:"); display.print(datas.mpu6050.ax, 1); // 强制保留1位小数，控制宽度

    // Y 轴：从 52 像素开始
    display.setCursor(52, 16);
    display.print("Y:"); display.print(datas.mpu6050.ay, 1);

    // Z 轴：从 90 像素开始，这样即使数据变大也不会换行
    display.setCursor(90, 16);
    display.print("Z:"); display.print(datas.mpu6050.az, 1);

    // 陀螺仪 Gyr
    display.setCursor(0, 26);
    display.print("G");
    display.setCursor(25, 26);
    display.print("X:"); display.print((int)datas.mpu6050.gx);
    display.setCursor(60, 26);
    display.print("Y:"); display.print((int)datas.mpu6050.gy);
    display.setCursor(95, 26);
    display.print("Z:"); display.print((int)datas.mpu6050.gz);

    display.drawFastHLine(0, 36, 128, SSD1306_WHITE);

    // --- 底部：气体与触发状态 ---
    // 气体数值条 (可视化展示)
    display.setCursor(0, 40);
    display.print("GAS:");
    display.print(datas.ags02_value);
    // 绘制一个小进度条
    int barWidth = map(datas.ags02_value, 0, 2000, 0, 50);
    display.drawRect(70, 40, 52, 7, SSD1306_WHITE);
    display.fillRect(71, 41, barWidth, 5, SSD1306_WHITE);

    // PIR 与 光敏2
    
    display.setCursor(0, 52);
    display.print("L1:"); display.print(datas.light1_adc_value);
    display.setCursor(65, 52);
    display.print("L2:"); display.print(datas.light2_adc_value);

    display.display();
}