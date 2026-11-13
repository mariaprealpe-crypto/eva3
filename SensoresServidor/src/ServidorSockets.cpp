#include "ServidorSockets.h"
#include "SensorTemperatura.h"
#include "SensorCaudal.h"
#include "SensorPresion.h"
#include "ExcepcionesSMTI.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <cctype>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_tipo;
    #define CERRAR_SOCKET closesocket
    #define FLAGS_ENVIO 0
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef socklen_t socklen_tipo;
    #define CERRAR_SOCKET close
    #define FLAGS_ENVIO MSG_NOSIGNAL
#endif

// Texto del ultimo error de red segun la plataforma.
static std::string errorRed() {
#ifdef _WIN32
    return "codigo Winsock " + std::to_string(WSAGetLastError());
#else
    return std::strerror(errno);
#endif
}

ServidorSockets::ServidorSockets(int puerto)
    : puerto(puerto), socketServidor(SOCKET_INVALIDO), winsockIniciado(false), siguienteId(1), activo(true),
      sensores(new ListaGen<Sensor*>()) {}

ServidorSockets::~ServidorSockets() {
    // La lista guarda punteros: primero se destruyen los sensores (polimorfismo
    // via destructor virtual) y luego la propia lista.
    while (!sensores->vacia()) {
        Sensor* s = sensores->eliminar(0);
        delete s;
    }
    delete sensores;
    if (socketServidor != SOCKET_INVALIDO) CERRAR_SOCKET(socketServidor);
#ifdef _WIN32
    if (winsockIniciado) WSACleanup();
#endif
}

Sensor* ServidorSockets::crearSensor(const std::string& tipo, const std::string& nombre) {
    std::string t = tipo;
    for (size_t i = 0; i < t.size(); ++i) t[i] = std::tolower((unsigned char)t[i]);

    if (t == "temperatura") return new SensorTemperatura(siguienteId++, nombre);
    if (t == "caudal")      return new SensorCaudal(siguienteId++, nombre);
    if (t == "presion")     return new SensorPresion(siguienteId++, nombre);
    throw ErrorComando("tipo de sensor desconocido '" + tipo +
                       "' (use temperatura, presion o caudal)");
}

int ServidorSockets::buscarIndicePorId(int id) {
    for (int i = 0; i < sensores->tamano(); ++i)
        if (sensores->obtener(i)->getId() == id) return i;
    throw ErrorSensor("no existe un sensor con id " + std::to_string(id));
}

std::string ServidorSockets::procesarComando(const std::string& linea) {
    std::istringstream in(linea);
    std::string cmd;
    in >> cmd;
    for (size_t i = 0; i < cmd.size(); ++i) cmd[i] = std::toupper((unsigned char)cmd[i]);

    std::ostringstream out;
    out << std::fixed << std::setprecision(2);

    if (cmd.empty()) {
        throw ErrorComando("linea vacia");
    } else if (cmd == "AYUDA") {
        out << "Comandos: AGREGAR <tipo> <nombre> | LISTA | LEER <id> | TODOS | "
               "ELIMINAR <id> | AYUDA | SALIR | APAGAR\n";
    } else if (cmd == "AGREGAR") {
        std::string tipo, nombre;
        in >> tipo >> nombre;
        if (tipo.empty() || nombre.empty())
            throw ErrorComando("uso: AGREGAR <tipo> <nombre>");
        Sensor* s = crearSensor(tipo, nombre);   // new en el heap
        try {
            sensores->agregar(s);
        } catch (...) {
            delete s;                            // evita fuga si algo falla
            throw;
        }
        out << "OK: creado " << s->descripcion() << "\n";
    } else if (cmd == "LISTA") {
        if (sensores->vacia()) out << "(sin sensores)\n";
        for (int i = 0; i < sensores->tamano(); ++i)
            out << sensores->obtener(i)->descripcion() << "\n";
    } else if (cmd == "LEER") {
        int id;
        if (!(in >> id)) throw ErrorComando("uso: LEER <id>");
        Sensor* s = sensores->obtener(buscarIndicePorId(id));
        out << s->getNombre() << ": " << s->leer() << " " << s->unidad() << "\n";  // polimorfismo
    } else if (cmd == "TODOS") {
        if (sensores->vacia()) out << "(sin sensores)\n";
        for (int i = 0; i < sensores->tamano(); ++i) {
            Sensor* s = sensores->obtener(i);
            out << s->getNombre() << " [" << s->tipo() << "]: "
                << s->leer() << " " << s->unidad() << "\n";
        }
    } else if (cmd == "ELIMINAR") {
        int id;
        if (!(in >> id)) throw ErrorComando("uso: ELIMINAR <id>");
        Sensor* s = sensores->eliminar(buscarIndicePorId(id));
        out << "OK: eliminado " << s->getNombre() << "\n";
        delete s;
    } else if (cmd == "APAGAR") {
        activo = false;
        out << "OK: servidor apagandose\n";
    } else {
        throw ErrorComando("comando desconocido '" + cmd + "' (escriba AYUDA)");
    }
    return out.str();
}

void ServidorSockets::enviar(SocketFd socketCliente, const std::string& texto) {
    size_t total = 0;
    while (total < texto.size()) {
        int n = (int)send(socketCliente, texto.c_str() + total, (int)(texto.size() - total), FLAGS_ENVIO);
        if (n < 0) throw ErrorRed("fallo en send(): " + errorRed());
        total += (size_t)n;
    }
}

void ServidorSockets::atenderCliente(SocketFd socketCliente) {
    enviar(socketCliente, "Servidor de sensores listo. Escriba AYUDA.\n");

    std::string buffer;
    char tmp[512];
    bool conectado = true;

    while (conectado && activo) {
        int n = (int)recv(socketCliente, tmp, (int)sizeof(tmp), 0);
        if (n < 0) throw ErrorRed("fallo en recv(): " + errorRed());
        if (n == 0) break;                       // el cliente cerro
        buffer.append(tmp, (size_t)n);

        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string linea = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);
            if (!linea.empty() && linea[linea.size() - 1] == '\r') linea.erase(linea.size() - 1);

            std::string cmdMayus = linea;
            for (size_t i = 0; i < cmdMayus.size(); ++i)
                cmdMayus[i] = std::toupper((unsigned char)cmdMayus[i]);
            if (cmdMayus == "SALIR") { enviar(socketCliente, "Hasta luego.\n"); conectado = false; break; }

            std::string respuesta;
            try {
                respuesta = procesarComando(linea);
            } catch (const ErrorSistema& e) {     // errores esperados del dominio
                respuesta = std::string("ERROR: ") + e.what() + "\n";
            } catch (const std::exception& e) {   // cualquier otra cosa
                respuesta = std::string("ERROR INTERNO: ") + e.what() + "\n";
            }
            enviar(socketCliente, respuesta);
            if (!activo) break;
        }
    }
}

void ServidorSockets::iniciar() {
#ifdef _WIN32
    WSADATA datos;
    if (WSAStartup(MAKEWORD(2, 2), &datos) != 0)
        throw ErrorRed("no se pudo inicializar Winsock (WSAStartup)");
    winsockIniciado = true;
#endif

    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor == SOCKET_INVALIDO)
        throw ErrorRed("no se pudo crear el socket: " + errorRed());

#ifndef _WIN32
    // En Linux permite reiniciar el servidor sin esperar (en Windows no se usa).
    int opt = 1;
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#endif

    sockaddr_in dir;
    std::memset(&dir, 0, sizeof(dir));
    dir.sin_family = AF_INET;
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    dir.sin_port = htons((unsigned short)puerto);

    if (bind(socketServidor, (sockaddr*)&dir, sizeof(dir)) < 0)
        throw ErrorRed("bind() fallo en el puerto " + std::to_string(puerto) + ": " + errorRed());
    if (listen(socketServidor, 5) < 0)
        throw ErrorRed("listen() fallo: " + errorRed());

    std::cout << "Servidor escuchando en el puerto " << puerto << std::endl;
}

void ServidorSockets::ejecutar() {
    while (activo) {
        sockaddr_in dirCliente;
        socklen_tipo largo = sizeof(dirCliente);
        SocketFd cliente = accept(socketServidor, (sockaddr*)&dirCliente, &largo);
        if (cliente == SOCKET_INVALIDO) {
            throw ErrorRed("accept() fallo: " + errorRed());
        }
        std::cout << "Cliente conectado: " << inet_ntoa(dirCliente.sin_addr) << std::endl;

        try {
            atenderCliente(cliente);
        } catch (const ErrorRed& e) {           // un cliente roto no tumba el servidor
            std::cerr << "Aviso: " << e.what() << std::endl;
        }
        CERRAR_SOCKET(cliente);
        std::cout << "Cliente desconectado" << std::endl;
    }
}
