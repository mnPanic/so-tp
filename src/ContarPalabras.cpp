#include <iostream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"
#include <time.h>

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cout << "Error: faltan argumentos." << std::endl;
        std::cout << std::endl;
        std::cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << std::endl;
        std::cout << "    " << "<archivo1> [<archivo2>...]" << std::endl;
        std::cout << std::endl;
        std::cout << "    threads_lectura: "
            << "Cantidad de threads a usar para leer archivos." << std::endl;
        std::cout << "    threads_maximo: "
            << "Cantidad de threads a usar para computar máximo." << std::endl;
        std::cout << "    archivo1, archivo2...: "
            << "Archivos a procesar." << std::endl;
        return 1;
    }
    int cantThreadsLectura = std::stoi(argv[1]);
    int cantThreadsMaximo = std::stoi(argv[2]);

    std::vector<std::string> filePaths = {};
    for (int i = 3; i < argc; i++) {
        filePaths.push_back(argv[i]);
    }

    HashMapConcurrente hashMap;
    timespec clockCargaInicio;
    timespec clockCargaFin;

    if (clock_gettime(CLOCK_REALTIME, &clockCargaInicio) != 0) {
        std::cerr << "Error creando clock de carga inicio" << std::endl;
        return -1;
    }
    cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);
    if (clock_gettime(CLOCK_REALTIME, &clockCargaFin) != 0) {
        std::cerr << "Error creando clock de carga fin" << std::endl;
        return -1;
    }

    timespec clockMaximoInicio;
    timespec clockMaximoFin;

    if (clock_gettime(CLOCK_REALTIME, &clockMaximoInicio) != 0) {
        std::cerr << "Error creando clock de máximo inicio" << std::endl;
        return -1;
    }
    auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);
    if (clock_gettime(CLOCK_REALTIME, &clockMaximoFin) != 0) {
        std::cerr << "Error creando clock de máximo fin" << std::endl;
        return -1;
    }

    std::cout << clockCargaFin.tv_nsec - clockCargaInicio.tv_nsec << std::endl;
    std::cout << clockMaximoFin.tv_nsec - clockMaximoInicio.tv_nsec << std::endl;
    std::cout << maximo.first << " " << maximo.second << std::endl;

    return 0;
}
