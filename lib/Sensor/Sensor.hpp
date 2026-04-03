#ifndef SENSOR_HPP
#define SENSOR_HPP
#include <string>
#include <ctime>
#include <Ticker.h>

typedef struct MpuData {
    double ax;
    double ay;
    double az;
    double gx;
    double gy;
    double gz;
} MpuData;

typedef struct DataTable_t {
    time_t time;
    MpuData mpu6050;
    float temperature;
    float humidity;
    int light1_adc_value;
    int light2_adc_value;
    bool pir;
    uint32_t ags02_value;
    float volume;
} DataTable_t;

class Sensor {
public:
    int interval;                   //传感器采样间隔
    static DataTable_t* dataTable;  //所有传感器类将采集的数据存储在同一个DataTable_t结构体中，方便后续处理和上传
    Ticker sensorTicker;            //传感器采样定时器
    bool isInit;                    //传感器是否初始化完成
public:
    Sensor(DataTable_t* dataTable,int interval);
    virtual ~Sensor();
    virtual void init() = 0;
    void start();
    void stop();
    virtual void readData() = 0;
};
#endif