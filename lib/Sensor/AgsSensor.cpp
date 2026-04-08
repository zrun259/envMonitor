#include "AgsSensor.hpp"

AgsSensor::AgsSensor(DataTable_t* dataTable, uint8_t address, TwoWire* wire, int interval)
    : Sensor(dataTable, interval), ags(address, wire), wire(wire), address(address), warmupTicker([this]() { this->checkWarmup(); }, 5000, 0, MILLIS) {
}

void AgsSensor::init() {
    if (this->wire == nullptr) {
        this->isInit = false;
        return;
    }

    this->isInit = false;

    if (!this->ags.begin()) {
        this->warmupTicker.stop();
        return;
    }

    this->ags.setUGM3Mode();
    this->warmupTicker.start();
}

void AgsSensor::checkWarmup() {
    if (this->ags.isHeated()) {
        this->isInit = true;
        this->warmupTicker.stop();
    }
}

void AgsSensor::readData() {
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    Sensor::dataTable->ags02_value = this->ags.readUGM3();
}
