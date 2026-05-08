#include "SoundSensor.hpp"

SoundSensor::SoundSensor(DataTable_t* dataTable, int interval)
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

    // 计算音量（dB）
    double sum_sq = 0;
    for (int i = 0; i < samplesRead; i++) {
        // 1. 归一化
        float normalized = (float)sBuffer[i] / 8388608.0; 
        // 2. 累加平方
        sum_sq += normalized * normalized;
    }

    // 3. 计算 RMS
    float rms = sqrt(sum_sq / samplesRead); 
    
    // 防止 log(0)
    if (rms < 1e-6) {
       Sensor::dataTable->volume = -120; 
       return;
    }; 

    // 4. 计算 dBFS
    float dbfs = 20 * log10(rms);
    
    // 5. 补偿为 dBSPL (根据手册灵敏度)
    float dbspl = dbfs + 70;
    Sensor::dataTable->volume = dbspl;
}