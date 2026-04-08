#ifndef MPU_SENSOR_HPP
#define MPU_SENSOR_HPP

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Sensor.hpp"

class MpuSensor : public Sensor {
public:
    MpuSensor(DataTable_t* dataTable, TwoWire* wire, uint8_t address = 0x68, int interval = 100);
    void init() override;
    void readData() override;
    void update(); // 新增：在主循环中调用的安全读取函数

private:
    Adafruit_MPU6050* mpu; // 关键修改：改为指针！
    TwoWire* wire;
    uint8_t address;
    volatile bool readyToRead; // 中断标志位
};

#endif