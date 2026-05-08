#ifndef INFO_CLIENT_HPP
#define INFO_CLIENT_HPP

#include <WiFi.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include "Sensor.hpp"
#include "time.h"
#include <string.h>

class InfoClient {
public:
    int sendInterval = 5000;    // 发送间隔（毫秒）
    bool isConnectedToWifi= false;   // WiFi连接状态
    bool isWebSocketConnected = false; // WebSocket连接状态
    bool sendOn = false;        // 是否启用定时发送
    volatile bool needSend = false; // Ticker 置位，loop 中消费
public:
    char ssid[32];
    char password[64];
    char SERVER_URL[128];
    DataTable_t* dataTable;
    Ticker sendTicker;
public:
    InfoClient(char* ssid, char* password, char* server_url,DataTable_t* dataTable,int sendInterval = 5000);
    void start();   //使能Ticker定时发送
    void stop();    //停止Ticker定时发送
    void init();    //连接WiFi和WebSocket
    void deinit();  //断开WiFi和WebSocket连接（为后续低功耗模式设计）
    void loop();    //在主循环中调用：维持WebSocket事件、按需发送
    void sendDataViaWS();   //通过WebSocket发送数据
private:
    WebSocketsClient webSocket;
    bool connectWifi(const char* ssid, const char* password);
    void disconnectWifi();
    bool isWifiConnected() const;
    void connectWebSocket();
    void disconnectWebSocket();
};


#endif
