#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Sensor.hpp"
#include <string.h>

class Client {
public:
    char ssid[32];
    char password[64];
    char SERVER_URL[128];
    DataTable_t* dataTable;
public:
    Client(char* ssid, char* password, char* server_url);
    void connect(const char* ssid, const char* password);
    void disconnect();
    bool isConnected() const;
    void sendData(DataTable_t* dataTable);
};


#endif
