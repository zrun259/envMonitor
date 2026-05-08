#include "SoundSensor.hpp"

SoundSensor::SoundSensor(DataTable_t* dataTable, uint8_t pin, int interval)
    : Sensor(dataTable, interval) {
}

void SoundSensor::init() {
    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    this->isInit = true;
}

void SoundSensor::readData() {
    if (Sensor::dataTable == nullptr || !this->isInit) {
        return;
    }

    size_t bytesRead = 0;
    i2s_read(I2S_PORT, sBuffer, bufferLen * sizeof(int32_t), &bytesRead, portMAX_DELAY);
    int samplesRead = bytesRead / sizeof(int32_t);

    // Calculate average volume
    int64_t sum = 0;
    for (int i = 0; i < samplesRead; i++) {
        sum += abs(sBuffer[i]);
    }
    Sensor::dataTable->volume = (float)sum / samplesRead;
}