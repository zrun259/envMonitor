#include "AgsSensor.hpp"

AgsSensor::AgsSensor(DataTable_t* dataTable, uint8_t address, TwoWire* wire, int interval)
    : Sensor(dataTable, interval), ags(address, wire), wire(wire), address(address) {
    // 构造函数保持简单，不再这里初始化 Ticker 逻辑
}

void AgsSensor::init() {
    if (this->wire == nullptr) {
        this->isInit = false;
        return;
    }

    this->isInit = false;

    if (!this->ags.begin()) {
        return;
    }

    this->ags.setUGM3Mode();

    // 使用静态转发函数
    this->warmupTicker.attach_ms(5000, AgsSensor::warmupCallback, this);
}

void AgsSensor::checkWarmup() {
    // 硬件 Ticker 会异步调用这里
    if (this->ags.isHeated()) {
        this->isInit = true;
        this->warmupTicker.detach(); // 预热完成，停止硬件定时器
        
        // 建议：预热完成后自动开启主采样定时器
        this->start(); 
    }
}

void AgsSensor::readData() {
    // 只有 isInit 为 true 时才会被主采样定时器调用
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    Sensor::dataTable->ags02_value = this->ags.readUGM3();
}