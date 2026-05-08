#ifndef DISPLAY_HPP
#define DISPLAY_HPP

class Display {
private:
    Adafruit_SSD1306* _screen;
    DataTable_t* _dt;
    uint8_t _brightness;
    volatile uint8_t _pageIndex; // 0: 环境主界面, 1: MPU6050详情
    const uint8_t _maxPages = 2;
    uint8_t _currentPage = 0;
    uint8_t _targetPage = 0;

    bool _isAnimating = false;
    int _animOffset = 0;     // 0~128
    uint8_t _animSpeed = 16; // 动画速度（可调）

public:
    Display(Adafruit_SSD1306* screen, DataTable_t* data) 
        : _screen(screen), _dt(data), _brightness(255), _pageIndex(0) {}

    void begin() {
        _screen->begin(SSD1306_SWITCHCAPVCC, 0x3C);
        _screen->clearDisplay();
        _screen->display();
    }

    // 设置亮度 (0-255)
    void setBrightness(uint8_t b) {
        _brightness = b;
        _screen->ssd1306_command(SSD1306_SETCONTRAST);
        _screen->ssd1306_command(_brightness);
    }

    // 中断调用的切换函数
    void IRAM_ATTR nextIPage() {
        uint8_t next = (_targetPage + 1) % _maxPages;

        // 启动动画
        _targetPage = next;
        _isAnimating = true;
        _animOffset = 0;
    }

    // 绘制环境主界面 (不含 MPU6050)
void drawMainPage(int offsetX) {

    // ===== 标题栏 =====
    _screen->drawLine(offsetX, 10, offsetX + 127, 10, SSD1306_WHITE);
    _screen->setCursor(offsetX + 2, 0);
    _screen->printf("HOME");

    // ===== PIR 呼吸灯 =====
    static uint8_t breath = 0;
    breath = (breath + 1) % 20;

    if (_dt->pir) {
        int r = 2 + abs(10 - breath) / 3;
        _screen->fillCircle(offsetX + 120, 4, r, SSD1306_WHITE);
    } else {
        _screen->drawCircle(offsetX + 120, 4, 3, SSD1306_WHITE);
    }

    // ===== 温度卡片 =====
    _screen->drawRect(offsetX + 0, 14, 60, 20, SSD1306_WHITE);
    _screen->setCursor(offsetX + 4, 16);
    _screen->printf("TEMP");
    _screen->setCursor(offsetX + 4, 24);
    _screen->printf("%.1fC", _dt->temperature);

    // ===== 湿度卡片 =====
    _screen->drawRect(offsetX + 68, 14, 60, 20, SSD1306_WHITE);
    _screen->setCursor(offsetX + 72, 16);
    _screen->printf("HUM");
    _screen->setCursor(offsetX + 72, 24);
    _screen->printf("%.1f%%", _dt->humidity);

    // ===== 光照进度条 =====
    int bar = map(4095 - (_dt->light1_adc_value+_dt->light2_adc_value)/2, 0, 4095, 0, 60);
    _screen->drawRect(offsetX + 0, 40, 60, 8, SSD1306_WHITE);
    _screen->fillRect(offsetX + 0, 40, bar, 8, SSD1306_WHITE);

    // ===== 音量卡片 =====
    _screen->drawRect(offsetX + 68, 40, 60, 20, SSD1306_WHITE);
    _screen->setCursor(offsetX + 72, 42);
    _screen->printf("VOL");
    _screen->setCursor(offsetX + 72, 50);
    _screen->printf("%.1fdB", _dt->volume);

    // ===== 底部状态 =====
    _screen->drawLine(offsetX, 54, offsetX + 52, 54, SSD1306_WHITE);

    _screen->setCursor(offsetX + 0, 56);
    _screen->printf("G:%d", _dt->ags02_value);

    //_screen->setCursor(offsetX + 10, 56);
    //_screen->printf(_dt->pir ? "ACTIVE" : "IDLE");
}

    // 绘制 MPU6050 详情页
void drawSensorPage(int offsetX) {

    _screen->drawLine(offsetX, 10, offsetX + 127, 10, SSD1306_WHITE);
    _screen->setCursor(offsetX + 2, 0);
    _screen->printf("MOTION");

    // ===== 姿态框 =====
    _screen->drawRect(offsetX + 34, 14, 60, 36, SSD1306_WHITE);

    int dotX = map(-_dt->mpu6050.ay, -10, 10, offsetX + 38, offsetX + 90);
    int dotY = map(-_dt->mpu6050.ax, -10, 10, 18, 44);

    _screen->fillCircle(
        constrain(dotX, offsetX + 38, offsetX + 90),
        constrain(dotY, 18, 44),
        3,
        SSD1306_WHITE
    );

    // ===== 数据（弱化显示）=====
    _screen->setCursor(offsetX + 0, 20);
    _screen->printf("X%2.1f", _dt->mpu6050.ax);

    _screen->setCursor(offsetX + 0, 30);
    _screen->printf("Y%2.1f", _dt->mpu6050.ay);

    _screen->setCursor(offsetX + 0, 40);
    _screen->printf("Z%2.1f", _dt->mpu6050.az);

    _screen->setCursor(offsetX + 104, 20);
    _screen->printf("%2.1f", _dt->mpu6050.gx);

    _screen->setCursor(offsetX + 104, 30);
    _screen->printf("%2.1f", _dt->mpu6050.gy);

    _screen->setCursor(offsetX + 104, 40);
    _screen->printf("%2.1f", _dt->mpu6050.gz);
}

void drawPage(uint8_t page, int offsetX) {
    if (page == 0) {
        drawMainPage(offsetX);
    } else {
        drawSensorPage(offsetX);
    }
}

    void update() {
    _screen->clearDisplay();
    _screen->setTextColor(SSD1306_WHITE);
    _screen->setTextSize(1);

    if (_isAnimating) {
        _animOffset += _animSpeed;

        if (_animOffset >= 128) {
            _animOffset = 0;
            _isAnimating = false;
            _currentPage = _targetPage;
        }

        // 当前页向左滑出
        drawPage(_currentPage, -_animOffset);

        // 新页从右滑入
        drawPage(_targetPage, 128 - _animOffset);

    } else {
        drawPage(_currentPage, 0);
    }

    _screen->display();
}
};

#endif // DISPLAY_HPP