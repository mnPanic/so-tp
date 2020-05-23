#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <atomic>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }

    while (file >> palabraActual) {
        hashMap.incrementar(palabraActual);
        cant++;
    }

    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


struct threadParams {
    const std::vector<std::string> &filePaths;
    std::atomic<int>* nextIndex;

    HashMapConcurrente& hashMap;
};

void* threadCargarArchivo(void* params) {
    threadParams p = *((threadParams*) params);

    unsigned int fileIndex = 0;
    while((fileIndex = p.nextIndex->fetch_add(1)) < p.filePaths.size()) {
        cargarArchivo(p.hashMap, p.filePaths[fileIndex]);
    }

    return NULL;
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    pthread_t tids[cantThreads];

    std::atomic<int> nextIndex(0);
    threadParams params = {
        .filePaths = filePaths,
        .nextIndex = &nextIndex,
        .hashMap   = hashMap,
    };

    for(unsigned int i = 0; i < cantThreads; i++) {
        pthread_create(&tids[i], NULL, threadCargarArchivo, &params);
    }

    for (unsigned int i = 0; i < cantThreads; i++) {
        pthread_join(tids[i], NULL);
    }
}

#endif
