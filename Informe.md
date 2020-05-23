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