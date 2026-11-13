#ifndef SENSORPRESION_H
#define SENSORPRESION_H

#include "Sensor.h"

class SensorPresion : public Sensor {
public:
    SensorPresion(int id, const std::string& nombre);
    double leer() override;
    std::string tipo() const override;
    std::string unidad() const override;
};

#endif
