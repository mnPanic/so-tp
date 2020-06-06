#include <iostream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"
#include <time.h>
#include <math.h>

timespec diff(timespec &start, timespec &end);
long timespecToMillis(timespec &time);
long timespecToMicros(timespec &time);

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

    timespec clockCarga = diff(clockCargaInicio, clockCargaFin);
    timespec clockMax = diff(clockMaximoInicio, clockMaximoFin);
    std::cout << timespecToMillis(clockCarga) << std::endl;
    std::cout << timespecToMicros(clockMax) << std::endl;
    std::cout << maximo.first << " " << maximo.second << std::endl;

    return 0;
}

timespec diff(timespec &start, timespec &end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

long timespecToMillis(timespec &time) {
    time_t s  = time.tv_sec;
    long ms = round(time.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    return s * 1000 + ms;
}

long timespecToMicros(timespec &time) {
    time_t s  = time.tv_sec;
    long us = round(time.tv_nsec / 1.0e3); // Convert nanoseconds to microseconds
    if (us > 999) {
        s++;
        us = 0;
    }

    return s * 1000000 + us;
}