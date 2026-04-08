#ifndef MPU_SENSOR_HPP
#define MPU_SENSOR_HPP

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Sensor.hpp"

class MpuSensor : public Sensor {
public:
    MpuSensor(DataTable_t* dataTable, TwoWire* wire, uint8_t address = 0x68, int interval = 1000);
    void init() override;
    void readData() override;

private:
    Adafruit_MPU6050 mpu;
    TwoWire* wire;
    uint8_t address;
};

#endif
