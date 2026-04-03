#include "DhtSensor.hpp"

DhtSensor::DhtSensor(DataTable_t* dataTable, uint8_t pin, uint8_t type, int interval)
    : Sensor(dataTable, interval), dht(pin, type) {
}

void DhtSensor::init() {
    dht.begin();
    this->isInit = true;
}

void DhtSensor::readData() {
    if (Sensor::dataTable == nullptr|| !this->isInit) {
        return;
    }

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // DHT occasionally returns NaN when the bus is noisy; keep last valid value.
    if (!isnan(humidity)) {
        Sensor::dataTable->humidity = humidity;
    }
    if (!isnan(temperature)) {
        Sensor::dataTable->temperature = temperature;
    }
}
