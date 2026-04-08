#ifndef PIR_SENSOR_HPP
#define PIR_SENSOR_HPP

#include <Arduino.h>
#include "Sensor.hpp"

class PirSensor : public Sensor {
public:
    PirSensor(DataTable_t* dataTable, uint8_t pirPin, int interval = 1000);
    void init() override;
    void readData() override;

private:
    uint8_t pirPin;
};

#endif
