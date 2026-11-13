#include "SensorCaudal.h"
#include "ExcepcionesSMTI.h"

SensorCaudal::SensorCaudal(int id, const std::string& nombre) : Sensor(id, nombre) {}

double SensorCaudal::leer() {
    double valor = aleatorio(5.0, 80.0);
    if (valor < 0.0 || valor > 500.0)
        throw ErrorSensor("lectura fuera del rango fisico en " + nombre);
    return valor;
}

std::string SensorCaudal::tipo() const { return "caudal"; }
std::string SensorCaudal::unidad() const { return "L/min"; }
