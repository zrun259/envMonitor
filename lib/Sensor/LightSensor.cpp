#include "LightSensor.hpp"

LightSensor::LightSensor(DataTable_t* dataTable, uint8_t light1AdcPin, uint8_t light2AdcPin, int interval)
    : Sensor(dataTable, interval), light1AdcPin(light1AdcPin), light2AdcPin(light2AdcPin) {
}

void LightSensor::init() {
    pinMode(this->light1AdcPin, INPUT);
    pinMode(this->light2AdcPin, INPUT);
    this->isInit = true;
}

void LightSensor::readData() {
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    Sensor::dataTable->light1_adc_value = analogRead(this->light1AdcPin);
    Sensor::dataTable->light2_adc_value = analogRead(this->light2AdcPin);
}
