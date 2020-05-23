#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <atomic>
#include <mutex>

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

    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; ++i) {
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

void HashMapConcurrente::lockAllEntries() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        locks[i].lock();    
    }
}

void HashMapConcurrente::unlockAllEntries() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        locks[i].unlock();    
    }
}


hashMapPair HashMapConcurrente::maximo() {
    hashMapPair max;
    max.second = 0;

    // Para evitar inconsistencias con insertar()
    lockAllEntries();

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max.second) {
                max = it.siguiente();
            }
        }
    }

    unlockAllEntries();

    return max;
}

hashMapPair HashMapConcurrente::findMax(unsigned int list) {
    hashMapPair max("", 0);

    locks[list].lock();
    for (
        auto it = tabla[list]->crearIt();
        it.haySiguiente();
        it.avanzar()
    ) {
        if (it.siguiente().second > max.second) {
            max = it.siguiente();
        }
    }
    locks[list].unlock();

    return max;
}

struct threadParams {
    HashMapConcurrente* hashMap;
    std::atomic<int> *nextList;
    std::mutex *maxMux;
    hashMapPair *currentMax;
};

void* threadMaximoParalelo(void* vparam) {
    threadParams p = *((threadParams*) vparam);
    unsigned int list = 0;
    hashMapPair max("", 0);

    while((list = p.nextList->fetch_add(1)) < HashMapConcurrente::cantLetras) {
        hashMapPair listMax = p.hashMap->findMax(list);
        if(listMax.second > max.second) {
            max = listMax;
        }
    }

    p.maxMux->lock();
    if(p.currentMax->second < max.second) {
        p.currentMax->first = max.first;
        p.currentMax->second = max.second;
    }
    p.maxMux->unlock();

    return NULL;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    pthread_t tids[cantThreads];

    std::mutex mux; 
    hashMapPair max;
    std::atomic<int> nextList(0);
    threadParams params = {
        .hashMap    = this,
        .nextList   = &nextList,
        .maxMux     = &mux,
        .currentMax = &max,
    };

    for(unsigned int i = 0; i < cantThreads; i++) {
        pthread_create(&tids[i], NULL, threadMaximoParalelo, &params);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_join(tids[i], NULL);
    }

    return max;
}

#endif
