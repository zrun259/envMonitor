#include "PirSensor.hpp"

PirSensor::PirSensor(DataTable_t* dataTable, uint8_t pirPin, int interval)
    : Sensor(dataTable, interval), pirPin(pirPin) {
}

void PirSensor::init() {
    pinMode(this->pirPin, INPUT);
    this->isInit = true;
}

void PirSensor::readData() {
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    Sensor::dataTable->pir = digitalRead(this->pirPin) == HIGH;
}
