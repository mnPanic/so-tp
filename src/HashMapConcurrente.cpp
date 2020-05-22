#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    int tableIndex = hashIndex(clave);
    ListaAtomica<hashMapPair> *entries = tabla[tableIndex];

    locks[tableIndex].lock();
    for (
        auto it = entries->crearIt();
        it.haySiguiente();
        it.avanzar()
    ) {
        if (it.siguiente().first == clave) {
            it.siguiente().second++;
            locks[tableIndex].unlock();
            return;
        }
    }

    hashMapPair newEntry = hashMapPair(clave, 1);
    entries->insertar(newEntry);
    locks[tableIndex].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> claves = std::vector<std::string>();

    for (int i = 0; i < HashMapConcurrente::cantLetras; ++i) {
        ListaAtomica<hashMapPair> *entries = tabla[i];
        for (
            auto it = entries->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            claves.push_back(it.siguiente().first);
        }
    }
    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int tableIndex = hashIndex(clave);
    ListaAtomica<hashMapPair> *entries = tabla[tableIndex];

    for (
        auto it = entries->crearIt();
        it.haySiguiente();
        it.avanzar()
    ) {
        if (it.siguiente().first == clave) {
            return it.siguiente().second;
        }
    }
    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }

    return *max;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
}

#endif
