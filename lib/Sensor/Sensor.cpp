#include "Sensor.hpp"

DataTable_t* Sensor::dataTable = nullptr;

Sensor::Sensor(DataTable_t* dataTable,int interval) : sensorTicker(std::bind(&Sensor::readData, this), interval * 1000, 0, MILLIS) {
    this->dataTable = dataTable;
    this->interval = interval;
    this->isInit = false;
}

void Sensor::start() {
    this->sensorTicker.start();
}

void Sensor::stop() {
    this->sensorTicker.stop();
}