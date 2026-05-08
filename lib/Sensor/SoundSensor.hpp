#ifndef SOUND_SENSOR_HPP
#define SOUND_SENSOR_HPP
#include "Sensor.hpp"
#include <driver/i2s.h>

// 1. 定义引脚
#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
// 2. 选择 I2S 端口
#define I2S_PORT I2S_NUM_0
// 3. 定义缓冲区大小
#define bufferLen 64

class SoundSensor : public Sensor {
public:
    int32_t sBuffer[bufferLen];
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = bufferLen
    };
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };
public:
    SoundSensor(DataTable_t* dataTable, uint8_t pin, int interval = 4000);
    void init() override;
    void readData() override;
};
#endif