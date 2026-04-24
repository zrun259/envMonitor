#include "Sensor.hpp"

DataTable_t* Sensor::dataTable = nullptr;

Sensor::Sensor(DataTable_t* dataTable,int interval){
    Sensor::dataTable = dataTable;
    this->interval = interval;
    this->isInit = false;
}

void Sensor::start() {
    this->sensorTicker.attach_ms(this->interval, Sensor::timerCallback, this);
}

void Sensor::stop() {
    this->sensorTicker.detach();
}