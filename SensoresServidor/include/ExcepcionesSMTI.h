#ifndef EXCEPCIONESSMTI_H
#define EXCEPCIONESSMTI_H

#include <stdexcept>
#include <string>

// Jerarquia de excepciones propia del proyecto.
class ErrorSistema : public std::runtime_error {
public:
    explicit ErrorSistema(const std::string& msg) : std::runtime_error(msg) {}
};

class ErrorSensor : public ErrorSistema {
public:
    explicit ErrorSensor(const std::string& msg) : ErrorSistema("Sensor: " + msg) {}
};

class ErrorLista : public ErrorSistema {
public:
    explicit ErrorLista(const std::string& msg) : ErrorSistema("Lista: " + msg) {}
};

class ErrorRed : public ErrorSistema {
public:
    explicit ErrorRed(const std::string& msg) : ErrorSistema("Red: " + msg) {}
};

class ErrorComando : public ErrorSistema {
public:
    explicit ErrorComando(const std::string& msg) : ErrorSistema("Comando: " + msg) {}
};

#endif
