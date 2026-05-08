#ifndef SENSOR_HPP
#define SENSOR_HPP
#include <string>
#include <ctime>
#include <Ticker.h>

typedef struct MpuData {
    double ax = 0.0f;
    double ay = 0.0f;
    double az = 0.0f;
    double gx = 0.0f;
    double gy = 0.0f;
    double gz = 0.0f;
} MpuData;

typedef struct DataTable_t {
    time_t time;
    bool isConnectedToWifi;
    bool isWebSocketConnected;
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
    int interval;                   //采样间隔
    static DataTable_t* dataTable;  //所有传感器类将采集的数据存储在同一数据表中
    Ticker sensorTicker;            //采样定时器
    bool isInit;                    //是否初始化完成标志
public:
    Sensor(DataTable_t* dataTable,int interval);    //构造函数，传入数据表指针和采样间隔
    virtual void init() = 0;                        //纯虚函数，子类必须实现初始化方法
    void start();                                   //启动采样定时器
    void stop();                                    //停止采样定时器        
    virtual void readData() = 0;                    //纯虚函数，子类必须实现读取数据方法    
    static void timerCallback(Sensor* instance) {
        if (instance) instance->readData();
    }
};
#endif