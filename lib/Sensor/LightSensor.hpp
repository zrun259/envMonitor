#ifndef LIGHT_SENSOR_HPP
#define LIGHT_SENSOR_HPP

#include <Arduino.h>
#include "Sensor.hpp"

class LightSensor : public Sensor {
public:
    LightSensor(DataTable_t* dataTable, uint8_t light1AdcPin, uint8_t light2AdcPin, int interval = 1000);
    void init() override;
    void readData() override;

private:
    uint8_t light1AdcPin;
    uint8_t light2AdcPin;
};

#endif
