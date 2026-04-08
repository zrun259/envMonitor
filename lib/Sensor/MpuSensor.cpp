#include "MpuSensor.hpp"

MpuSensor::MpuSensor(DataTable_t* dataTable, TwoWire* wire, uint8_t address, int interval)
    : Sensor(dataTable, interval), wire(wire), address(address) {
}

void MpuSensor::init() {
    if (this->wire == nullptr) {
        this->isInit = false;
        return;
    }
    // 增加调试信息
    this->isInit = mpu.begin(this->address, this->wire);
    if (this->isInit) {
        Serial.println("MPU6050 Init Success!");
    } else {
        Serial.print("MPU6050 Init Failed at address 0x");
        Serial.println(this->address, HEX);
    }
}

void MpuSensor::readData() {
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    sensors_event_t a;
    sensors_event_t g;
    sensors_event_t temp;
    mpu.getEvent(&a, &g, &temp);

    Sensor::dataTable->mpu6050.ax = a.acceleration.x;
    Sensor::dataTable->mpu6050.ay = a.acceleration.y;
    Sensor::dataTable->mpu6050.az = a.acceleration.z;
    Sensor::dataTable->mpu6050.gx = g.gyro.x;
    Sensor::dataTable->mpu6050.gy = g.gyro.y;
    Sensor::dataTable->mpu6050.gz = g.gyro.z;
}
