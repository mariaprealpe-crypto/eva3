#include "SensorPresion.h"
#include "ExcepcionesSMTI.h"

SensorPresion::SensorPresion(int id, const std::string& nombre) : Sensor(id, nombre) {}

double SensorPresion::leer() {
    double valor = aleatorio(980.0, 1040.0);
    if (valor < 300.0 || valor > 1100.0)
        throw ErrorSensor("lectura fuera del rango fisico en " + nombre);
    return valor;
}

std::string SensorPresion::tipo() const { return "presion"; }
std::string SensorPresion::unidad() const { return "hPa"; }
