#ifndef SERVIDORSOCKETS_H
#define SERVIDORSOCKETS_H

#include <string>

// Soporte multiplataforma: Windows (Winsock2) y Linux (sockets POSIX).
#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET SocketFd;
    #define SOCKET_INVALIDO INVALID_SOCKET
#else
    typedef int SocketFd;
    #define SOCKET_INVALIDO (-1)
#endif
#include "ListaGen.h"
#include "Sensor.h"

class ServidorSockets {
private:
    int puerto;
    SocketFd socketServidor;
    bool winsockIniciado;
    int siguienteId;
    bool activo;
    ListaGen<Sensor*>* sensores;   // lista en el HEAP

    Sensor* crearSensor(const std::string& tipo, const std::string& nombre);
    int buscarIndicePorId(int id);
    std::string procesarComando(const std::string& linea);
    void atenderCliente(SocketFd socketCliente);
    void enviar(SocketFd socketCliente, const std::string& texto);

public:
    explicit ServidorSockets(int puerto);
    ~ServidorSockets();

    void iniciar();     // socket + bind + listen (lanza ErrorRed)
    void ejecutar();    // bucle accept
};

#endif
