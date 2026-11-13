#ifndef LISTAGEN_H
#define LISTAGEN_H

#include <string>
#include "ExcepcionesSMTI.h"

// TAD generico: lista simplemente enlazada. Todos los nodos viven en el HEAP.
// La lista NO es duena de los objetos apuntados cuando T es un puntero;
// quien la use debe liberar esos objetos (ver ServidorSockets).
template <typename T>
class ListaGen {
private:
    struct Nodo {
        T dato;
        Nodo* sig;
        Nodo(const T& d) : dato(d), sig(nullptr) {}
    };

    Nodo* cabeza;
    Nodo* cola;
    int cantidad;

    // Evita copias superficiales (doble liberacion).
    ListaGen(const ListaGen&);
    ListaGen& operator=(const ListaGen&);

public:
    ListaGen() : cabeza(nullptr), cola(nullptr), cantidad(0) {}

    ~ListaGen() { vaciar(); }

    void agregar(const T& dato) {
        Nodo* nuevo = new Nodo(dato);      // memoria dinamica
        if (cola == nullptr) cabeza = cola = nuevo;
        else { cola->sig = nuevo; cola = nuevo; }
        ++cantidad;
    }

    T& obtener(int indice) {
        if (indice < 0 || indice >= cantidad)
            throw ErrorLista("indice fuera de rango (" + std::to_string(indice) + ")");
        Nodo* aux = cabeza;
        for (int i = 0; i < indice; ++i) aux = aux->sig;
        return aux->dato;
    }

    // Elimina el nodo y devuelve el dato que contenia.
    T eliminar(int indice) {
        if (indice < 0 || indice >= cantidad)
            throw ErrorLista("no se puede eliminar, indice invalido (" + std::to_string(indice) + ")");
        Nodo* previo = nullptr;
        Nodo* actual = cabeza;
        for (int i = 0; i < indice; ++i) { previo = actual; actual = actual->sig; }
        if (previo == nullptr) cabeza = actual->sig;
        else previo->sig = actual->sig;
        if (actual == cola) cola = previo;
        T dato = actual->dato;
        delete actual;
        --cantidad;
        return dato;
    }

    int tamano() const { return cantidad; }
    bool vacia() const { return cantidad == 0; }

    void vaciar() {
        while (cabeza != nullptr) {
            Nodo* tmp = cabeza;
            cabeza = cabeza->sig;
            delete tmp;
        }
        cola = nullptr;
        cantidad = 0;
    }
};

#endif
