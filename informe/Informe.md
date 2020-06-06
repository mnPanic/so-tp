# TP-SO

_**Disclaimer: esto es solo un draft, que se deja de lado a favor del latex**_

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

Para ilustrar un escenario. imaginemos el siguiente código de un potencial
usuario de nuestro hashmap atómico (en pseudocódigo)

```python
# thread principal
map = HashMapConcurrente()

thread(
  for 1 to 50:
    map.insertar("ardilla")

  for 1 to 21:
    map.insertar("zorro")
)

thread(map.maximo())
```

Donde `thread` corre lo especificado en un thread.

Todos los `"ardilla"` irían a parar a la misma lista, la correspondiente al slot
0 del hash map, y los `"zorro"` al ultimo.

Para algún scheduling, podría suceder que el thread A, que hace las inserciones,
ejecute hasta insertar 20 veces ardilla, y luego sea desalojado por el scheduler
a favor de B. Este llega a recorrer toda la lista de la letra A, y toma a
ardilla como el máximo actual con valor 20, y luego recorre las listas
siguientes. Luego le toca nuevamente a A que hace las inserciones de zorro, y
finaliza su ejecición.

```python
# Thread A        # Thread B
# inserciones
1  ardilla
2  ardilla
...
20 ardilla        # <---- punto hasta el que ve maximo del slot 0
...
50 ardilla
1  zorro
2  zorro
...
21 zorro          # <---- punto hasta el que ve maximo del ultimo slot
```

Cuando ejecute B hasta finalizar, se va a encontrar que zorro tiene 21
apariciones, que es mayor a las 20 que tenía de ardilla, y lo tomará como
máximo.

Pero para el thread principal que ejecuta primero las 50 inserciones de ardilla
y luego las 21 de zorro, nunca fué el máximo, y ardilla siempre lo fué.

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

La implementación de cargarMultiplesArchivos muy similar a maximoParalelo, solo
que con menos recursos. La lista de files no hace falta que tenga ningun
mecanismo de proteccion ya que solo se lee.

## Ejercicio 5

En este punto, buscaremos evaluar qu´e ventajas ofrece, en t´erminos de performance,
la ejecuci´on concurrente a la hora de encontrar la palabra con mayor cantidad de
apariciones en un conjunto de archivos. Elaboren en primer lugar una hip´otesis,
bas´andose en los conocimientos que poseen sobre el tema.

* Suponemos que aumentar la cantidad de threads mejorará pero solo hasta cierto
  punto.
* Claramente en maximo no sirve tener mas que la cantidad de entries en
  la tabla (26)
* Disminuye la performance al lanzar mas threads que cores tiene la maquina.
* 4 puede llegar a andar peor que 3