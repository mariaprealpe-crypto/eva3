# SensoresServidor (version Windows / ZinjaI)

Proyecto C++ con:
- Clase base abstracta `Sensor` y derivadas `SensorTemperatura`, `SensorPresion`, `SensorCaudal` (polimorfismo).
- TAD generico `ListaGen<T>` (lista enlazada en el Heap) que almacena `Sensor*`.
- Servidor TCP en el puerto 8080 (Winsock2) con `try / catch / throw` (jerarquia `ErrorSistema`).

## Compilar en Windows
ZinjaI: abrir `SensoresServidor.zpr` y pulsar F9 (Ejecutar).
Si ZinjaI no reconoce el .zpr, en Opciones del proyecto agregar al enlazador: `-lws2_32`.

Consola (MinGW / MSYS2):

    g++ -Wall -std=c++11 src/*.cpp -Iinclude -o servidor.exe -lws2_32
    servidor.exe

IMPORTANTE: `-lws2_32` es obligatorio en Windows (libreria de sockets).
El Firewall de Windows puede preguntar si permite el acceso: aceptar.

## Probar
Con el servidor corriendo, en otra ventana (PowerShell o CMD):

    telnet localhost 8080

(Si no existe: Panel de control > Activar o desactivar caracteristicas de Windows > Cliente Telnet.)
Alternativas: `ncat localhost 8080` (Nmap) o PuTTY en modo "Raw" al puerto 8080.

Comandos: `AGREGAR <tipo> <nombre>`, `LISTA`, `LEER <id>`, `TODOS`, `ELIMINAR <id>`, `AYUDA`, `SALIR`, `APAGAR`.
Tipos: temperatura, presion, caudal.

El mismo codigo tambien compila en Linux/WSL sin `-lws2_32`.
