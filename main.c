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

char* cambiar_mapa(char* actual, char* mapa1, char* mapa2, char* mapa3, int x, int y );

int contar_celdas_libres(char* actual, int filas, int columnas);

void generar_ventana(char *matriz, char *salida, int columnas, int fila_inicio,
                     int columna_inicio, int tam_ventana);

int contar_caracter(const char *mapa, int total_celdas, char caracter);
int validar_movimiento(const char *mapa, int columnas, int fila, int columna,
                       int tiene_llave);
int detectar_objeto(const char *mapa, int columnas, int fila, int columna,
                    char objeto);

void iniciar_mapa(char mapa[FILAS][COLUMNAS], int *fila_jugador,
                  int *columna_jugador) {
  const char *dibujo[FILAS] = {
      "############################################################",
      "#P..M.K....................................................#",
      "#..........................................................#",
      "#..........................................................#",
      "#.........########............########.....................#",
      "#..........................................................#",
      "#.............#.........#.........#.........#..............#",
      "#.............#.........#.........#.........#..............#",
      "#.............#.........#.........#.........#..............#",
      "#..........................................................#",
      "#.....................################.....................#",
      "#..........................................................#",
      "#..........................................................#",
      "#...........######........................######...........#",
      "#...........D....#........................#....#...........#",
      "#...........#....#......####....####......#....#...........#",
      "#...........#....#......#.........#.......#....#...........#",
      "#...........######......#.........#.......######...........#",
      "#....................M.....................................#",
      "#.........########............########.....................#",
      "#..........................................................#",
      "#....#.........#.........#.........#.........#.........#...#",
      "#....#.........#.........#.........#.........#.........#...#",
      "#....#.........#.........#.........#.........#.........#...#",
      "#..........................................................#",
      "#.....................################.....................#",
      "#..........................................................#",
      "#..........................................................#",
      "#..................########....########..........M.........#",
      "#.......................#........#.........................#",
      "#...........######......#........#......######.............#",
      "#...........#....#......#........#......#....#.............#",
      "#...........######..########....########..######...........#",
      "#..........................................................#",
      "#..........................................................#",
      "#.........########............########.....................#",
      "#..........................................................#",
      "#.............#.........#.........#.........#..............#",
      "#.............#.........#.........#.........#..............#",
      "#..........................................................#",
      "#.............######....................######......M......#",
      "#.............#.............................#..............#",
      "#.............#........######....######.....#..............#",
      "#.............#........#....#....#....#.....#..............#",
      "#.............######...######....######..######............#",
      "#..........................................................#",
      "#.....................################.....................#",
      "#..........................................................#",
      "#....#.........#.........#.........#.........#.........#...#",
      "#....#.........#.........#.........#.........#.........#...#",
      "#..........................................................#",
      "#.....................################.....................#",
      "#.....................#..............#.....................#",
      "#...........######....#..............#....######...........#",
      "#...........#....#....#..............#....#....#...........#",
      "#...........######....################....######....M......#",
      "#..........................................................#",
      "#..........................................................#",
      "#..........................................................#",
      "############################################################",
  };

  for (int i = 0; i < FILAS; i++) {
    for (int j = 0; j < COLUMNAS; j++) {
      mapa[i][j] = dibujo[i][j];

      if (mapa[i][j] == JUGADOR) {
        *fila_jugador = i;
        *columna_jugador = j;
      }
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
                    int columna_jugador, int monedas_recolectadas, int llaves) {
  char salida[TAM_SALIDA];
  int fila_inicio = inicio_ventana(fila_jugador);
  int columna_inicio = inicio_ventana(columna_jugador);

  generar_ventana(&mapa[0][0], salida, COLUMNAS, fila_inicio, columna_inicio,
                  VENTANA);

  printf("Monedas: %d | Llaves: %d\n", monedas_recolectadas, llaves);
  printf("%s", salida);
}

int main() {
  char mapa[FILAS][COLUMNAS];
  int fila_jugador;
  int columna_jugador;
  int llaves = 0;
  int monedas_recolectadas = 0;
  int total_monedas;
  char direccion;

  iniciar_mapa(mapa, &fila_jugador, &columna_jugador);
  total_monedas = contar_caracter(&mapa[0][0], FILAS * COLUMNAS, MONEDA);

  do {
    int fila_nueva = fila_jugador;
    int columna_nueva = columna_jugador;

    system("cls");
    imprimir_juego(mapa, fila_jugador, columna_jugador, monedas_recolectadas,
                   llaves);

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

    if (!validar_movimiento(&mapa[0][0], COLUMNAS, fila_nueva, columna_nueva,
                            llaves > 0)) {
      continue;
    }

    if (detectar_objeto(&mapa[0][0], COLUMNAS, fila_nueva, columna_nueva,
                        MONEDA) &&
        monedas_recolectadas < total_monedas) {
      monedas_recolectadas++;
    }

    if (detectar_objeto(&mapa[0][0], COLUMNAS, fila_nueva, columna_nueva,
                        LLAVE)) {
      llaves++;
    }

    mapa[fila_jugador][columna_jugador] = CAMINO;
    mapa[fila_nueva][columna_nueva] = JUGADOR;

    fila_jugador = fila_nueva;
    columna_jugador = columna_nueva;
  } while (1);

  return 0;
}
