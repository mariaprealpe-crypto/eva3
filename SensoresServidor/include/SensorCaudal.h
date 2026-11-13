#ifndef SENSORCAUDAL_H
#define SENSORCAUDAL_H

#include "Sensor.h"

class SensorCaudal : public Sensor {
public:
    SensorCaudal(int id, const std::string& nombre);
    double leer() override;
    std::string tipo() const override;
    std::string unidad() const override;
};

#endif
