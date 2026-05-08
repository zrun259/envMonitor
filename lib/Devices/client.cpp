#include "client.hpp"


InfoClient::InfoClient(char* ssid, char* password, char* server_url) {
    strncpy(this->ssid, ssid, sizeof(this->ssid) - 1);
    this->ssid[sizeof(this->ssid) - 1] = '\0';

    strncpy(this->password, password, sizeof(this->password) - 1);
    this->password[sizeof(this->password) - 1] = '\0';

    strncpy(this->SERVER_URL, server_url, sizeof(this->SERVER_URL) - 1);
    this->SERVER_URL[sizeof(this->SERVER_URL) - 1] = '\0';
}

void InfoClient::connect(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void InfoClient::disconnect() {
    WiFi.disconnect();
}

bool InfoClient::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void InfoClient::sendData(DataTable_t* dataTable) {
    if (dataTable == nullptr) {
        return;
    }

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Serialize data to JSON
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["time"] = dataTable->time;
    jsonDoc["temperature"] = dataTable->temperature;
    jsonDoc["humidity"] = dataTable->humidity;
    jsonDoc["light1_adc_value"] = dataTable->light1_adc_value;
    jsonDoc["light2_adc_value"] = dataTable->light2_adc_value;
    jsonDoc["pir"] = dataTable->pir;
    jsonDoc["ags02_value"] = dataTable->ags02_value;
    jsonDoc["volume"] = dataTable->volume;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    int httpResponseCode = http.POST(jsonString);
    http.end();
}
