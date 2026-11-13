#ifndef SENSOR_H
#define SENSOR_H

#include <string>

// Clase base ABSTRACTA (tiene metodos virtuales puros).
class Sensor {
protected:
    int id;
    std::string nombre;

    static double aleatorio(double minimo, double maximo);

public:
    Sensor(int id, const std::string& nombre);
    virtual ~Sensor();                       // destructor virtual

    int getId() const;
    const std::string& getNombre() const;

    virtual double leer() = 0;               // lectura simulada
    virtual std::string tipo() const = 0;
    virtual std::string unidad() const = 0;
    virtual std::string descripcion() const; // comportamiento por defecto
};

#endif
