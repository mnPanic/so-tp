# TP-SO

## Ejercicio 1

* Una operación es atómica cuando la misma resulta indivisible. Más 
específicamente, esto quiere decir que un Context Switch no podría ocurrir 
durante la ejecución de dicha operación.
* La lista sería atómica si todas sus operaciones pudieran ser ejecutadas de 
forma atómica.
* En particular, esta lista no resulta atómica. Es fácil verlo puesto que hay 
varias operaciones en esta lista que no lo son, como por ejemplo `longitud`; no
es indivisible.
* `insertar`, en este caso,


¿Qué significa que la lista sea atómica? ¿Si un programa utiliza esta lista atómica, queda protegido de incurrir en condiciones de carrera? ¿Cómo hace su implementación de insertar para cumplir la propiedad de atomicidad?

## Ejercicio 2

En el punto a. analizamos los tipos de Race Conditions que pueden afectar a la
ejecución de la función en cuestión. Notamos que si no hubiera colisión de hash,
no habría problemas de condiciones de carrera pues las listas empleadas para
cada bucket son distintas. Por lo tanto, analizaremos los posibles casos para
cuando hay colisión de hash.

* Cuando las claves son distintas, notamos que en ningún caso habría race 
conditions, pues:
  * Si ambas claves existen, se modificarán valores de nodos distintos.
  * Si ambas claves son inexistentes, se insertarán de manera atómica.
  * Si una clave es inexistente y la otra existente, se insertará de manera 
    atómica y modificará el valor de un nodo ya existente. El órden en el que se
    ejecuten estas operaciones resulta irrelevante.
* Cuando las claves son iguales, surgen los siguientes inconvenientes:
  * Si la clave no existe, podría ocurrir que se la inserte dos veces.
  * Si la clave existe, podría ocurrir que se pierda uno de los dos incrementos
    a la misma.

Para salvar estos casos, planteamos la utilización de un lock particular para
cada lista.

En el punto b. y c. no utilizamos ninguna primitiva de sync porque no se pueden
generar race conditions en las lecturas (gracias a que `insertar`) es atómico,
por lo que las implementaciones en estos casos resultan triviales.

## Ejercicio 3

a) ¿Qu´e problemas puede ocasionar que maximo e incrementar se ejecuten
concurrentemente? Piensen un escenario de ejecuci´on en que maximo devuelva un
resultado que no fue, en ning´un momento, el m´aximo de la tabla.

```text
thread A      thread B
maximo()      for(i = 1...50) insertar("avion")

thread A                  thread B
maximo()
  max en lista 1 (a)
  .                       for(i = 1...50) insertar("avion")
  max en lista 2
  ....
```

// TODO: preguntar que quiere decir "en ningun momento"

b) Describan brevemente su implementaci´on de maximoParalelo. ¿Cu´al fue
la estrategia elegida para repartir el trabajo entre los threads? ¿Qu´e recursos
son compartidos por los threads? ¿C´omo hicieron para proteger estos
recursos de condiciones de carrera?

Para implementar maximoParalelo, pensamos en hacer que un hilo principal lance
la cantidad de threads indicadas, y que cada uno sea autónomo, es decir, que
busque el máximo una lista que no haya sido revisada todavía, y que cambie
el máximo actual en caso de que el que encontró sea mayor.

Los recursos necesarios fueron los siguientes

* `HashMapConcurrente* hashMap`: Un detalle de la implementación, es que como
  `pthreads` no soporta correr en un thread un método de un objeto, tuvimos que
  hacer un *workaround* con una función wrapper en el medio que tome esto
  como parametro.
* `std::atomic<int> *nextList`: La siguiente lista a recorrer para buscar el
  máximo. Basta con un atomic para protegerlo, ya que solamente es necesario
  hacer `getAndInc` (`fetch_add` en `std::atomic`).
* `std::mutex *maxMux` y `hashMapPair *currentMax`: El maximo actual global.
  Como necesitabamos que fuera de acceso concurrente, y la operación a realizar
  no podía resolverse con un atomic (comparar por mayor y en caso de que si
  reemplazar), recurrimos a el good ol mutex.

Como optimización, en realidad cada thread no chequea si su máximo es mayor al
actual después de ver cada lista, sino que lo hace al final, cuando ya no tiene
más listas por ver. De esta forma, nos evitamos locks innecesarios.

## Ejercicio 4

Para la implementación de `cargarArchivo` no fue necesario tomar ningún tipo de
recaudo nuevo, pues se utiliza sólamente `incrementar` que ya implementamos de
thread safe.