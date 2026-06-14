#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define FILAS 60
#define COLUMNAS 60
#define VENTANA 20
#define TAM_SALIDA (VENTANA * (VENTANA + 1) + 1)

#define CAMINO '.'
#define MURO '#'
#define JUGADOR 'P'
#define MONEDA 'M'
#define LLAVE 'K'
#define PUERTA 'D'
#define ESCALERA 'E'

#include "mapa.h"

char *cambiar_mapa(char *actual, char *mapa1, char *mapa2, char *mapa3,
                   int fila, int columna);

void generar_ventana(const char *matriz, char *salida, int columnas, int fila_inicio,
                     int columna_inicio, int tam_ventana);

int contar_caracter(const char *mapa, int total_celdas, char caracter);
int validar_movimiento(const char *mapa, int columnas, int fila, int columna,
                       int tiene_llave);
int detectar_objeto(const char *mapa, int columnas, int fila, int columna,
                    char objeto);

void copiar_mapa(const char origen[FILAS][COLUMNAS], char mapa[FILAS][COLUMNAS],
                 int usar_jugador_inicial, int *fila_jugador,
                 int *columna_jugador) {
  for (int i = 0; i < FILAS; i++) {
    for (int j = 0; j < COLUMNAS; j++) {
      char celda = origen[i][j] == '\0' ? MURO : origen[i][j];

      if (celda == JUGADOR && usar_jugador_inicial) {
        *fila_jugador = i;
        *columna_jugador = j;
      } else if (celda == JUGADOR) {
        celda = CAMINO;
      }

      mapa[i][j] = celda;
    }
  }
}

int posicion_valida(int fila, int columna) {
  if (fila < 0 || fila >= FILAS) {
    return 0;
  }

  if (columna < 0 || columna >= COLUMNAS) {
    return 0;
  }

  return 1;
}

int inicio_ventana(int posicion) {
  int inicio = posicion - (VENTANA / 2);

  if (inicio < 0) {
    return 0;
  }

  if (inicio > FILAS - VENTANA) {
    return FILAS - VENTANA;
  }

  return inicio;
}

void imprimir_juego(const char mapa[FILAS][COLUMNAS], int fila_jugador,
                    int columna_jugador, int monedas_recolectadas, int llaves,
                    int piso_actual) {
  char salida[TAM_SALIDA];
  int fila_inicio = inicio_ventana(fila_jugador);
  int columna_inicio = inicio_ventana(columna_jugador);

  generar_ventana(&mapa[0][0], salida, COLUMNAS, fila_inicio, columna_inicio,
                  VENTANA);

  printf("Piso: %d | Monedas: %d | Llaves: %d\n", piso_actual,
         monedas_recolectadas, llaves);
  printf("%s", salida);
}

int main() {
  char mapa1[FILAS][COLUMNAS];
  char mapa2[FILAS][COLUMNAS];
  char mapa3[FILAS][COLUMNAS];
  char (*mapa_actual)[COLUMNAS] = mapa1;
  int fila_jugador;
  int columna_jugador;
  int piso_actual = 1;
  int llaves = 0;
  int monedas_recolectadas = 0;
  int total_monedas;
  char celda_jugador = CAMINO;
  char direccion;

  copiar_mapa(nivel1, mapa1, 1, &fila_jugador, &columna_jugador);
  copiar_mapa(nivel2, mapa2, 0, NULL, NULL);
  copiar_mapa(nivel3, mapa3, 0, NULL, NULL);

  total_monedas = contar_caracter(&mapa1[0][0], FILAS * COLUMNAS, MONEDA) +
                  contar_caracter(&mapa2[0][0], FILAS * COLUMNAS, MONEDA) +
                  contar_caracter(&mapa3[0][0], FILAS * COLUMNAS, MONEDA);

  do {
    int fila_nueva = fila_jugador;
    int columna_nueva = columna_jugador;

    system("cls");
    imprimir_juego(mapa_actual, fila_jugador, columna_jugador,
                   monedas_recolectadas, llaves, piso_actual);

    direccion = tolower(_getch());

    if (direccion == 'q') {
      break;
    } else if (direccion == 'w') {
      fila_nueva--;
    } else if (direccion == 's') {
      fila_nueva++;
    } else if (direccion == 'a') {
      columna_nueva--;
    } else if (direccion == 'd') {
      columna_nueva++;
    } else {
      continue;
    }

    if (!posicion_valida(fila_nueva, columna_nueva)) {
      continue;
    }

    if (!validar_movimiento(&mapa_actual[0][0], COLUMNAS, fila_nueva,
                            columna_nueva, llaves > 0)) {
      continue;
    }

    if (detectar_objeto(&mapa_actual[0][0], COLUMNAS, fila_nueva, columna_nueva,
                        ESCALERA)) {
      char *mapa_anterior = &mapa_actual[0][0];
      char *mapa_siguiente = cambiar_mapa(mapa_anterior, &mapa1[0][0],
                                          &mapa2[0][0], &mapa3[0][0],
                                          fila_nueva, columna_nueva);

      if (mapa_siguiente != mapa_anterior) {
        mapa_actual[fila_jugador][columna_jugador] = celda_jugador;
        mapa_actual = (char(*)[COLUMNAS])mapa_siguiente;

        if (mapa_siguiente == &mapa1[0][0]) {
          piso_actual = 1;
          fila_jugador = 1;
          columna_jugador = 2;
        } else if (mapa_siguiente == &mapa2[0][0]) {
          piso_actual = 2;
          if (fila_nueva == 0 && columna_nueva == 56) {
            fila_jugador = 58;
            columna_jugador = 7;
          } else {
            fila_jugador = 1;
            columna_jugador = 2;
          }
        } else {
          piso_actual = 3;
          fila_jugador = 1;
          columna_jugador = 56;
        }

        celda_jugador = mapa_actual[fila_jugador][columna_jugador];
        mapa_actual[fila_jugador][columna_jugador] = JUGADOR;
        continue;
      }
    }

    if (detectar_objeto(&mapa_actual[0][0], COLUMNAS, fila_nueva, columna_nueva,
                        MONEDA) &&
        monedas_recolectadas < total_monedas) {
      monedas_recolectadas++;
      mapa_actual[fila_nueva][columna_nueva] = CAMINO;
    }

    if (detectar_objeto(&mapa_actual[0][0], COLUMNAS, fila_nueva, columna_nueva,
                        LLAVE)) {
      llaves++;
      mapa_actual[fila_nueva][columna_nueva] = CAMINO;
    }

    mapa_actual[fila_jugador][columna_jugador] = celda_jugador;
    celda_jugador = mapa_actual[fila_nueva][columna_nueva];
    if (celda_jugador == PUERTA) {
      celda_jugador = CAMINO;
    }
    mapa_actual[fila_nueva][columna_nueva] = JUGADOR;

    fila_jugador = fila_nueva;
    columna_jugador = columna_nueva;
  } while (1);

  return 0;
}
