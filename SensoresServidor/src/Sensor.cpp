#include "Sensor.h"
#include <cstdlib>
#include <sstream>

Sensor::Sensor(int id, const std::string& nombre) : id(id), nombre(nombre) {}
Sensor::~Sensor() {}

int Sensor::getId() const { return id; }
const std::string& Sensor::getNombre() const { return nombre; }

double Sensor::aleatorio(double minimo, double maximo) {
    double r = static_cast<double>(std::rand()) / RAND_MAX;
    return minimo + r * (maximo - minimo);
}

std::string Sensor::descripcion() const {
    std::ostringstream os;
    os << "[" << id << "] " << nombre << " (" << tipo() << ", " << unidad() << ")";
    return os.str();
}
