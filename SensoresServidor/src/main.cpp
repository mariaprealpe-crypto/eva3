#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ServidorSockets.h"
#include "ExcepcionesSMTI.h"

int main() {
    std::srand((unsigned)std::time(nullptr));

    ServidorSockets* servidor = nullptr;
    try {
        servidor = new ServidorSockets(8080);
        servidor->iniciar();
        servidor->ejecutar();
    } catch (const ErrorSistema& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
        delete servidor;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Excepcion inesperada: " << e.what() << std::endl;
        delete servidor;
        return 1;
    }
    delete servidor;
    std::cout << "Servidor finalizado correctamente." << std::endl;
    return 0;
}
