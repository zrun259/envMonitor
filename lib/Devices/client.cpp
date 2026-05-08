#include "client.hpp"

static InfoClient* s_instance = nullptr;

static void timerCallback(InfoClient* instance) {
    if (instance && instance->sendOn) instance->needSend = true;
}

static void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    if (!s_instance) return;
    switch (type) {
        case WStype_CONNECTED:
            s_instance->isWebSocketConnected = true;
            Serial.printf("[WS] Connected to %s\n", (char*)payload);
            break;
        case WStype_DISCONNECTED:
            s_instance->isWebSocketConnected = false;
            Serial.println("[WS] Disconnected");
            break;
        case WStype_TEXT:
            Serial.printf("[WS] Recv: %.*s\n", (int)length, (char*)payload);
            break;
        default:
            break;
    }
}

InfoClient::InfoClient(char* ssid, char* password, char* server_url,
                       DataTable_t* dataTable, int sendInterval) {
    strncpy(this->ssid, ssid, sizeof(this->ssid) - 1);
    this->ssid[sizeof(this->ssid) - 1] = '\0';
    strncpy(this->password, password, sizeof(this->password) - 1);
    this->password[sizeof(this->password) - 1] = '\0';
    strncpy(this->SERVER_URL, server_url, sizeof(this->SERVER_URL) - 1);
    this->SERVER_URL[sizeof(this->SERVER_URL) - 1] = '\0';
    this->dataTable    = dataTable;
    this->sendInterval = sendInterval;
    s_instance = this;
}

void InfoClient::init() {
    if (!connectWifi(this->ssid, this->password)) {
        Serial.println("[InfoClient] WiFi connect failed");
        return;
    }
    connectWebSocket();
}

void InfoClient::deinit() {
    stop();
    disconnectWebSocket();
    disconnectWifi();
}

void InfoClient::start() {
    this->sendOn = true;
    this->sendTicker.attach_ms(this->sendInterval, ::timerCallback, this);
}

void InfoClient::stop() {
    this->sendOn = false;
    this->sendTicker.detach();
}

bool InfoClient::connectWifi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.printf("[WiFi] Connecting to %s", ssid);
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(300);
        Serial.print('.');
    }
    Serial.println();
    this->isConnectedToWifi = (WiFi.status() == WL_CONNECTED);
    if (this->isConnectedToWifi) {
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
        this->dataTable->isConnectedToWifi = true;

        configTime(8 * 3600, 0, "ntp.aliyun.com", "pool.ntp.org");
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            Serial.println("Time synced!");
        }
    }
    return this->isConnectedToWifi;
}

void InfoClient::disconnectWifi() {
    WiFi.disconnect(true);
    this->isConnectedToWifi = false;
    this->dataTable->isConnectedToWifi = false;
}

bool InfoClient::isWifiConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void InfoClient::connectWebSocket() {
    // 解析 SERVER_URL，支持 "ws://host:port/path" 或 "host:port/path"
    const char* p = this->SERVER_URL;
    if (strncmp(p, "ws://", 5) == 0)  p += 5;
    if (strncmp(p, "wss://", 6) == 0) p += 6;

    char host[64] = {0};
    char path[64] = "/";
    uint16_t port = 81;

    const char* slash = strchr(p, '/');
    const char* colon = strchr(p, ':');

    size_t hostLen = 0;
    if (colon && (!slash || colon < slash)) {
        hostLen = colon - p;
        port = (uint16_t)atoi(colon + 1);
    } else {
        hostLen = slash ? (size_t)(slash - p) : strlen(p);
    }
    if (hostLen >= sizeof(host)) hostLen = sizeof(host) - 1;
    memcpy(host, p, hostLen);

    if (slash) {
        strncpy(path, slash, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
    }

    Serial.printf("[WS] begin %s:%u%s\n", host, port, path);
    this->webSocket.begin(host, port, path);
    this->webSocket.onEvent(::webSocketEvent);
    this->webSocket.setReconnectInterval(5000);
}

void InfoClient::disconnectWebSocket() {
    this->webSocket.disconnect();
    this->isWebSocketConnected = false;
    this->dataTable->isWebSocketConnected = false;
}

void InfoClient::loop() {
    this->webSocket.loop();
    if (this->needSend) {
        this->needSend = false;
        sendDataViaWS();
    }
}

void InfoClient::sendDataViaWS() {
    if (!this->dataTable) return;
    if (!this->isWebSocketConnected) return;

    JsonDocument doc;
    time_t now;
    time(&now);
    doc["time"]        = now;
    doc["temperature"] = this->dataTable->temperature;
    doc["humidity"]    = this->dataTable->humidity;
    doc["light1"]      = this->dataTable->light1_adc_value;
    doc["light2"]      = this->dataTable->light2_adc_value;
    doc["pir"]         = this->dataTable->pir;
    doc["ags02"]       = this->dataTable->ags02_value;
    doc["volume"]      = this->dataTable->volume;

    JsonObject mpu = doc["mpu6050"].to<JsonObject>();
    mpu["ax"] = this->dataTable->mpu6050.ax;
    mpu["ay"] = this->dataTable->mpu6050.ay;
    mpu["az"] = this->dataTable->mpu6050.az;
    mpu["gx"] = this->dataTable->mpu6050.gx;
    mpu["gy"] = this->dataTable->mpu6050.gy;
    mpu["gz"] = this->dataTable->mpu6050.gz;

    char buf[512];
    size_t n = serializeJson(doc, buf, sizeof(buf));
    this->webSocket.sendTXT(buf, n);
}
