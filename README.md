# Trabajo Práctico 1 - Threading

## Integrantes

Integrante              | LU
----------------------- | --
Manuel Panichelli       | 72/18
Vladimir Pomsztein      | 364/18
Gaston Einan Rosinov    | 37/18

## Compilar

Para compilar el tp, basta con

    make

Desde el root del proyecto.

## Tests

- Para correr los tests de forma usual

      make test

- Para correr los tests varias veces (y dar mas confianza de que no hay
  condiciones de carrera)

      ./src/runner.sh | tqdm --total 100000

## Experimentación

Se encuentra en el notebook `notebooks/ExperimentacionThreads`, basta con
correrlo para obtener los mismos resultados.

## Informe

El informe fue hecho en [overleaf](https://www.overleaf.com/read/rydfpmkzchxy)
y se encuentra una copia en el directorio [informe](informe)
