#ifndef DISPLAY_HPP
#define DISPLAY_HPP

class Display {
private:
    Adafruit_SSD1306* _screen;
    DataTable_t* _dt;
    uint8_t _brightness;
    volatile uint8_t _pageIndex; // 0: 环境主界面, 1: MPU6050详情
    const uint8_t _maxPages = 2;

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
        _pageIndex = (_pageIndex + 1) % _maxPages;
    }

    // 绘制环境主界面 (不含 MPU6050)
    void drawMainPage() {
        _screen->setCursor(0, 0);
        _screen->printf("--- ENV MONITOR ---");
        
        _screen->setCursor(0, 16);
        _screen->printf("Temp: %.1f C", _dt->temperature);
        _screen->setCursor(0, 26);
        _screen->printf("Humd: %.1f %%", _dt->humidity);
        
        _screen->setCursor(0, 40);
        _screen->printf("L1: %d  L2: %d", _dt->light1_adc_value, _dt->light2_adc_value);
        
        _screen->setCursor(0, 54);
        _screen->printf("Gas: %d  PIR: %s", _dt->ags02_value, _dt->pir ? "ACT" : "IDLE");
    }

    // 绘制 MPU6050 详情页
    void drawSensorPage() {
        _screen->setCursor(0, 0);
        _screen->printf("- MPU6050 DETAIL -");
        
        _screen->setCursor(0, 14);
        _screen->printf("A X:%-4.1f Y:%-4.1f", _dt->mpu6050.ax, _dt->mpu6050.ay);
        _screen->setCursor(92, 14); // 绝对坐标防止换行
        _screen->printf("Z:%-4.1f", _dt->mpu6050.az);

        _screen->setCursor(0, 28);
        _screen->printf("G X:%-4d Y:%-4d", (int)_dt->mpu6050.gx, (int)_dt->mpu6050.gy);
        _screen->setCursor(92, 28);
        _screen->printf("Z:%-4d", (int)_dt->mpu6050.gz);

        // 底部做一个动态小球或者姿态指示（可选）
        _screen->drawRect(10, 45, 108, 15, SSD1306_WHITE);
        int dotX = map(-(int)_dt->mpu6050.ay, -10, 10, 12, 116);
        _screen->fillCircle(constrain(dotX, 12, 116), 52, 4, SSD1306_WHITE);
    }

    void update() {
        _screen->clearDisplay();
        _screen->setTextColor(SSD1306_WHITE);
        _screen->setTextSize(1);

        if (_pageIndex == 0) {
            drawMainPage();
        } else {
            drawSensorPage();
        }

        _screen->display();
    }
};

#endif // DISPLAY_HPP