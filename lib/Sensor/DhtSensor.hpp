#ifndef DHT_SENSOR_HPP
#define DHT_SENSOR_HPP

#include <Arduino.h>
#include <DHT.h>
#include "Sensor.hpp"

class DhtSensor : public Sensor {
public:
    DhtSensor(DataTable_t* dataTable, uint8_t pin, uint8_t type, int interval = 4000);
    void init() override;
    void readData() override;

private:
    DHT dht;
};

#endif
