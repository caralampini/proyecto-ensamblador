# BitQuest: Explorador de Matrices con C y NASM

Proyecto final de Lenguaje Ensamblador. Es un videojuego de consola desarrollado con C y NASM de 64 bits.

## Requisitos

- Windows de 64 bits
- NASM
- GCC/MinGW de 64 bits

## Compilacion

Desde la carpeta del proyecto, ejecutar:

```bat
compilar.bat
```

El script compila los archivos `.asm` con NASM y enlaza los archivos `.c` con GCC.

Comandos equivalentes:

```bat
nasm -f win64 procesos.asm -o procesos.obj
gcc main.c juego.c procesos.obj -o proyecto-ensamblador.exe
```

## Ejecucion

Despues de compilar:

```bat
proyecto-ensamblador.exe
```

## Controles

- `W`: mover hacia arriba
- `A`: mover hacia la izquierda
- `S`: mover hacia abajo
- `D`: mover hacia la derecha
- `Q`: salir del juego

## Objetivo del Juego

El jugador debe recorrer mapas grandes representados con matrices de 60 x 60. En pantalla solo se muestra una ventana visible de 20 x 20 alrededor del jugador.

Durante el juego se deben recolectar monedas, encontrar llaves, abrir puertas y llegar a las salidas para avanzar entre niveles. Al completar el tercer nivel se muestra el resumen final con monedas, pasos y puntaje.

## Simbolos del Mapa

- `#`: pared
- `.`: camino libre
- `P`: jugador
- `M`: moneda
- `K`: llave
- `D`: puerta
- `E`: salida
- `X`: enemigo

## Funciones en NASM

El proyecto usa rutinas en NASM llamadas desde C:

- `contar_caracter`: cuenta cuantas veces aparece un caracter en el mapa.
- `validar_movimiento`: valida si el jugador puede moverse a una celda.
- `detectar_objeto`: detecta si una celda contiene un objeto especifico.
- `contar_celdas_libres`: cuenta las celdas libres representadas con `.`.
- `calcular_puntaje`: calcula el puntaje final usando monedas, pasos y niveles completados.

Formula del puntaje:

```txt
puntaje = monedas * 100 + niveles * 1000 - pasos
```

Si el resultado es negativo, el puntaje final se ajusta a `0`.

## Notas

El proyecto esta implementado para Windows. Usa `conio.h`, `_getch()`, `system("cls")` y convencion de llamadas Windows x64 en NASM.
