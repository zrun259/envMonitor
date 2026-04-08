#ifndef AGS_SENSOR_HPP
#define AGS_SENSOR_HPP

#include <Arduino.h>
#include <AGS02MA.h>
#include <Wire.h>
#include "Sensor.hpp"

class AgsSensor : public Sensor {
public:
    AgsSensor(DataTable_t* dataTable, uint8_t address, TwoWire* wire, int interval = 1000);
    void init() override;
    void readData() override;

private:
    void checkWarmup();

    AGS02MA ags;
    TwoWire* wire;
    uint8_t address;
    Ticker warmupTicker;
};

#endif
