#ifndef SENSORTEMPERATURA_H
#define SENSORTEMPERATURA_H

#include "Sensor.h"

class SensorTemperatura : public Sensor {
public:
    SensorTemperatura(int id, const std::string& nombre);
    double leer() override;
    std::string tipo() const override;
    std::string unidad() const override;
};

#endif
