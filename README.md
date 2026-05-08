# envMonitor

基于 ESP32 + PlatformIO + Arduino 框架的多传感器环境监测终端。采样本地环境数据，在 OLED 屏上循环显示，并通过 WebSocket 定时上传到上位机。

## 硬件与传感器

| 模块 | 型号 / 类型 | 备注 |
| --- | --- | --- |
| 主控 | ESP32 Dev Module | `board = esp32dev` |
| 显示 | SSD1306 OLED (128x64, I2C) | 硬件 I2C (Wire) |
| 温湿度 | DHT11 | GPIO18 |
| 光敏 | 双路 ADC | GPIO39 / GPIO34 |
| 人体感应 | PIR | GPIO35 |
| 加速度 / 陀螺仪 | MPU6050 | 软件 I2C: SDA=17, SCL=16 |
| 气体 | AGS02MA | 软件 I2C, 地址 0x1A |
| 声音 | 模拟麦克风 | 在 `SoundSensor` 中定义 |
| 按钮 | GPIO2, 上拉 + 下降沿中断 | 切换 OLED 页面 |

## 目录结构

```
envMonitor/
├── platformio.ini          # 板卡与库依赖
├── src/main.cpp            # 入口：实例化所有传感器 / 屏幕 / 客户端
├── lib/Sensor/             # 传感器基类 + 各外设派生类
├── lib/Devices/
│   ├── client.{hpp,cpp}    # InfoClient: WiFi + WebSocket 上传
│   └── display.hpp         # OLED 多页显示
└── test/
    ├── server.js           # 上位机测试服务器（Node.js / ws）
    └── README
```

## 固件烧录

需要安装 [PlatformIO](https://platformio.org/)（VSCode 插件或 CLI）。

1. 在 `src/main.cpp` 顶部修改 WiFi 凭据和上位机地址：
   ```cpp
   #define WIFI_SSID      "your-ssid"
   #define WIFI_PASSWORD  "your-password"
   ```
2. 在实例化 `InfoClient` 处传入上位机地址，例如 `ws://192.168.1.100:8080/`。
3. 连接 ESP32，执行：
   ```bash
   pio run -t upload
   pio device monitor -b 115200
   ```

## 运行上位机测试服务器

测试服务器位于 `test/server.js`，监听 **8080** 端口，把 ESP32 发来的 JSON 按表格打印。

```bash
cd test
npm install ws
node server.js
```

看到 `WebSocket 服务端已启动在端口 8080...` 即可，ESP32 连上后会开始打印每一帧数据。

> 注意：上位机和 ESP32 必须在同一局域网，ESP32 端的 `SERVER_URL` 要填运行 `server.js` 的那台机器的局域网 IP + `:8080`。

## 数据格式

`InfoClient::sendDataViaWS()` 序列化出的 JSON 字段（见 `lib/Devices/client.cpp`）：

```json
{
  "time": 1746700000,
  "temperature": 25.3,
  "humidity": 48.2,
  "light1": 1234,
  "light2": 980,
  "pir": false,
  "ags02": 560,
  "volume": 0.12,
  "mpu6050": { "ax":0.01,"ay":0.00,"az":0.98,"gx":0.1,"gy":-0.2,"gz":0.0 }
}
```

`time` 是通过 NTP（`ntp.aliyun.com`, `pool.ntp.org`）同步后的 UTC+8 时间戳。

## 修改采样 / 上传频率

所有周期性任务都基于 `Ticker`，频率通过构造函数参数（单位毫秒）传入。

### 调整单个传感器的采样频率

在 `src/main.cpp` 中，每个传感器实例构造函数的**最后一个参数**就是采样间隔：

```cpp
DhtSensor    dht(&datas, 18, DHT11, 4000);   // DHT11 每 4 秒采样一次
LightSensor  light(&datas, 39, 34, 1000);    // 光敏每 1 秒
PirSensor    pir(&datas, 35, 500);           // PIR 每 0.5 秒
AgsSensor    ags(&datas, 0x1A, &Wire1, 4000);
MpuSensor    mpu(&datas, &Wire1, 0x68, 100); // MPU6050 每 0.1 秒
SoundSensor  sound(&datas, 1000);
```

改数字即可。过于频繁会挤占总线 / CPU，DHT11 物理上 1~2 秒以内不建议。

### 调整上传频率

`InfoClient` 的第 5 个参数是上传间隔（毫秒，默认 5000）：

```cpp
InfoClient client(WIFI_SSID, WIFI_PASSWORD,
                  "ws://192.168.1.100:8080/",
                  &datas,
                  2000);  // 每 2 秒发一次
```

也可以运行期动态修改：先 `client.stop();` 再改 `client.sendInterval`，最后 `client.start();`。

### 增加新的传感器

1. 在 `lib/Sensor/` 新建 `FooSensor.{hpp,cpp}`，继承 `Sensor`，实现 `init()` 和 `readData()`（`readData` 里把结果写进 `dataTable`）。
2. 在 `lib/Sensor/Sensor.hpp` 的 `DataTable_t` 中加字段。
3. `main.cpp` 里实例化并调用 `foo.init(); foo.start();`。
4. 如果要上传新字段，在 `InfoClient::sendDataViaWS()` 里把字段加进 JSON。

## 主循环依赖

`InfoClient` 的 Ticker 回调只把 `needSend` 置位，实际的 WebSocket 事件处理与发送都在 `InfoClient::loop()` 里完成，因此必须在 `loop()` 中调用它：

```cpp
void loop() {
    screen.update();
    client.loop();   // 不调用则收不到连接事件，也不会发包
    delay(16);
}
```
