#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <mutex>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
   public:
      static const unsigned int cantLetras = 26;

      HashMapConcurrente();

      void incrementar(std::string clave);
      std::vector<std::string> claves();
      unsigned int valor(std::string clave);

      hashMapPair maximo();
      hashMapPair maximoParalelo(unsigned int cantThreads);

      hashMapPair findMax(unsigned int list);

   private:
      ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
      std::mutex locks[HashMapConcurrente::cantLetras];

      void lockAllEntries();
      void unlockAllEntries();

      static unsigned int hashIndex(std::string clave);
};

#endif  /* HMC_HPP */
