#include "MpuSensor.hpp"

MpuSensor::MpuSensor(DataTable_t* dataTable, TwoWire* wire, uint8_t address, int interval)
    : Sensor(dataTable, interval), wire(wire), address(address) {
    this->mpu = nullptr; // 初始化为空指针
    this->readyToRead = false;
}

void MpuSensor::init() {
    if (this->wire == nullptr) return;

    Serial.println("--- MPU6050 Internal Init Starting ---");

    // 关键修改：在这里真正实例化对象，确保在 setup() 中执行
    if (this->mpu == nullptr) {
        this->mpu = new Adafruit_MPU6050(); 
    }

    // 尝试初始化，千万不要用 while 死循环！
    this->isInit = this->mpu->begin(this->address, this->wire);

    if (this->isInit) {
        Serial.println("MPU6050 Init SUCCESS!");
        this->mpu->setAccelerometerRange(MPU6050_RANGE_8_G);
        this->mpu->setGyroRange(MPU6050_RANGE_500_DEG);
        this->mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
    } else {
        Serial.println("MPU6050 Init FAILED!");
        // 初始化失败就不管它了，不要死循环
    }
}

// 这是由硬件 Ticker (中断) 触发的，绝对不能在这里操作 I2C
void MpuSensor::readData() {
    this->readyToRead = true; 
}

// 这是在主程序的 loop() 中调用的，安全！
void MpuSensor::update() {
    if (this->readyToRead && this->isInit && this->mpu != nullptr && Sensor::dataTable != nullptr) {
        sensors_event_t a, g, temp;
        // 安全读取 I2C 数据
        if (this->mpu->getEvent(&a, &g, &temp)) {
            Sensor::dataTable->mpu6050.ax = a.acceleration.x;
            Sensor::dataTable->mpu6050.ay = a.acceleration.y;
            Sensor::dataTable->mpu6050.az = a.acceleration.z;
            Sensor::dataTable->mpu6050.gx = g.gyro.x;
            Sensor::dataTable->mpu6050.gy = g.gyro.y;
            Sensor::dataTable->mpu6050.gz = g.gyro.z;
        }
        this->readyToRead = false; // 清除标志位
    }
}