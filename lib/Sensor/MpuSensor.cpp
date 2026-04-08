#include "MpuSensor.hpp"

MpuSensor::MpuSensor(DataTable_t* dataTable, TwoWire* wire, uint8_t address, int interval)
    : Sensor(dataTable, interval), wire(wire), address(address) {
}

void MpuSensor::init() {
    if (this->wire == nullptr) {
        this->isInit = false;
        return;
    }

    this->isInit = mpu.begin(this->address, this->wire);
    this->isInit = true;
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
