#include "SensorTemperatura.h"
#include "ExcepcionesSMTI.h"

SensorTemperatura::SensorTemperatura(int id, const std::string& nombre) : Sensor(id, nombre) {}

double SensorTemperatura::leer() {
    double valor = aleatorio(15.0, 35.0);
    if (valor < -50.0 || valor > 100.0)
        throw ErrorSensor("lectura fuera del rango fisico en " + nombre);
    return valor;
}

std::string SensorTemperatura::tipo() const { return "temperatura"; }
std::string SensorTemperatura::unidad() const { return "C"; }
