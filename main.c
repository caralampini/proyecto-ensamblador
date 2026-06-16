#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include "mapa.h"

#define FILAS 60
#define COLUMNAS 60
#define VENTANA 20
#define TAM_SALIDA (VENTANA * (VENTANA + 1) + 1)

#define VERDE "#23D11B" //jugador
#define ROJO "#D12323" //enemigos
#define AMARILLO "#D1D123" //monedas
#define AZUL "#2375D1" //puertas
#define MAGENTA "#D123D1" //llave


#define CAMINO '.'
#define MURO '#'
#define JUGADOR 'P'
#define MONEDA 'M'
#define LLAVE 'K'
#define PUERTA 'D'
#define ESCALERA 'E'
#define ENEMIGO 'X'
#define ENEMIGO_MAPA 'x'
#define AREA_PERSECUCION 3 // distancia max a la que un enemigo puede perseguir al jugador
#define MAX_ENEMIGOS 64 // cantidad max de enemigos que se pueden manejar

//correcciones acerca de contar_caracter y generar_ventana, se corrigieron 
//los nombres de las funciones en el header y en el asm, se corrigió la función contar_caracter para que cuente correctamente
bool verificar_puerta_llave(bool* llavero, int puerta, int tamañoVector);

char *cambiar_mapa(char *actual, char *mapa1, char *mapa2, char *mapa3,
                   int fila, int columna);

int contar_celdas_libres(char* actual, int numCeldas);//numCeldas == filas*columnas

//estructura para persecucion

typedef struct {
  int fila;
  int columna;
  char bajo;
} Enemigo;

//funciones para el manejo de enemigos
void cargar_enemigos(char mapa[FILAS][COLUMNAS], Enemigo enemigos[], int *num_enemigos);
int esta_en_misma_habitacion(const char mapa[FILAS][COLUMNAS], int fila_a, int col_a,
                            int fila_b, int col_b);
int enemigo_puede_perseguir(const char mapa[FILAS][COLUMNAS], int fila_e, int col_e,
                           int fila_j, int col_j);
int mover_enemigo(char mapa[FILAS][COLUMNAS], Enemigo *enemigo,
                  int fila_jugador, int columna_jugador);
int mostrar_game_over(void);

void generar_ventana(const char *matriz, char *salida, int columnas, int fila_inicio,
                     int columna_inicio, int tam_ventana);

int contar_caracter(char *mapa, int total_celdas, char caracter);
int validar_movimiento(const char *mapa, int columnas, int fila, int columna,
                       int tiene_llave);
int detectar_objeto(const char *mapa, int columnas, int fila, int columna,
                    char objeto);

//convierte los defines hex a atributos de consola (mejor coincidencia posible)
static WORD attr_from_hex(const char *hex) {
  if (!hex) return 0;
  if (strcmp(hex, VERDE) == 0) return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  if (strcmp(hex, ROJO) == 0) return FOREGROUND_RED | FOREGROUND_INTENSITY;
  if (strcmp(hex, AMARILLO) == 0) return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  if (strcmp(hex, AZUL) == 0) return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  if (strcmp(hex, MAGENTA) == 0) return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  return 0;
}

static WORD get_color_for_char(char ch) {
  switch (ch) {
  case CAMINO: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // blanco
  case MURO: return attr_from_hex(AZUL);
  case JUGADOR: return attr_from_hex(VERDE);
  case MONEDA: return attr_from_hex(AMARILLO);
  case LLAVE: return attr_from_hex(MAGENTA);
  case PUERTA: return attr_from_hex(AZUL);
  case ESCALERA: return FOREGROUND_RED | FOREGROUND_GREEN; // oscuro
  case ENEMIGO: return attr_from_hex(ROJO);
  default: return 0;
  }
}

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

//carga el mapa con los enemigos y devuelve la cantidad de enemigos encontrados
//hice toda la tarde esto ayudenme
void cargar_enemigos(char mapa[FILAS][COLUMNAS], Enemigo enemigos[], int *num_enemigos) {
  *num_enemigos = 0;
  
  for (int i = 0; i < FILAS; i++) {
    for (int j = 0; j < COLUMNAS; j++) {
      if (mapa[i][j] == ENEMIGO) {
        if (*num_enemigos < MAX_ENEMIGOS) {
          enemigos[*num_enemigos].fila = i;
          enemigos[*num_enemigos].columna = j;
          enemigos[*num_enemigos].bajo = CAMINO;
          (*num_enemigos)++;
        }
        mapa[i][j] = CAMINO;
      }
    }
  }

  for (int k = 0; k < *num_enemigos; k++) {
    mapa[enemigos[k].fila][enemigos[k].columna] = ENEMIGO;
  }
}

int esta_en_misma_habitacion(const char mapa[FILAS][COLUMNAS], int fila_a, int col_a,
                            int fila_b, int col_b) {
  if (fila_a == fila_b && col_a == col_b) {
    return 1;
  }

  unsigned char visit[FILAS][COLUMNAS] = {0};
  int qf[FILAS * COLUMNAS];
  int qc[FILAS * COLUMNAS];
  int head = 0;
  int tail = 0;

  visit[fila_a][col_a] = 1;
  qf[tail] = fila_a;
  qc[tail] = col_a;
  tail++;

  const int dr[4] = {-1, 1, 0, 0};
  const int dc[4] = {0, 0, -1, 1};

  while (head < tail) {
    int rf = qf[head];
    int rc = qc[head];
    head++;

    for (int d = 0; d < 4; d++) {
      int nf = rf + dr[d];
      int nc = rc + dc[d];

      if (nf < 0 || nf >= FILAS || nc < 0 || nc >= COLUMNAS) {
        continue;
      }
      if (visit[nf][nc]) {
        continue;
      }

      char celda = mapa[nf][nc];
      if (celda == MURO || celda == PUERTA) {
        continue;
      }

      if (nf == fila_b && nc == col_b) {
        return 1;
      }

      visit[nf][nc] = 1;
      qf[tail] = nf;
      qc[tail] = nc;
      tail++;
    }
  }

  return 0;
}

int enemigo_puede_perseguir(const char mapa[FILAS][COLUMNAS], int fila_e, int col_e,
                           int fila_j, int col_j) {
  if (!esta_en_misma_habitacion(mapa, fila_e, col_e, fila_j, col_j)) {
    return 0;
  }

  int distancia = abs(fila_e - fila_j) + abs(col_e - col_j);
  return distancia <= AREA_PERSECUCION;
}

int mover_enemigo(char mapa[FILAS][COLUMNAS], Enemigo *enemigo,
                  int fila_jugador, int columna_jugador) {
  const int dr[4] = {-1, 1, 0, 0};
  const int dc[4] = {0, 0, -1, 1};
  int opciones[4];
  int num_opciones = 0;
  int mejor_dist = FILAS + COLUMNAS;
  int mejores[4];
  int num_mejores = 0;
  int distancia_jugador = abs(enemigo->fila - fila_jugador) + abs(enemigo->columna - columna_jugador);
  int puede_cazar = enemigo_puede_perseguir(mapa, enemigo->fila, enemigo->columna,
                                            fila_jugador, columna_jugador);

  for (int d = 0; d < 4; d++) {
    int nf = enemigo->fila + dr[d];
    int nc = enemigo->columna + dc[d];

    if (nf < 0 || nf >= FILAS || nc < 0 || nc >= COLUMNAS) {
      continue;
    }

    char destino = mapa[nf][nc];
    if (destino == MURO || destino == PUERTA || destino == ENEMIGO) {
      continue;
    }

    if (!esta_en_misma_habitacion(mapa, enemigo->fila, enemigo->columna, nf, nc)) {
      continue;
    }

    opciones[num_opciones++] = d;

    if (puede_cazar) {
      int distancia = abs(nf - fila_jugador) + abs(nc - columna_jugador);
      if (distancia < mejor_dist) {
        mejor_dist = distancia;
        num_mejores = 0;
        mejores[num_mejores++] = d;
      } else if (distancia == mejor_dist) {
        mejores[num_mejores++] = d;
      }
    }
  }

  if (num_opciones == 0) {
    return 0;
  }

  int dir;
  if (puede_cazar && num_mejores > 0 && distancia_jugador <= AREA_PERSECUCION) {
    dir = mejores[rand() % num_mejores];
  } else {
    dir = opciones[rand() % num_opciones];
  }

  int nueva_fila = enemigo->fila + dr[dir];
  int nueva_columna = enemigo->columna + dc[dir];
  char destino = mapa[nueva_fila][nueva_columna];

  mapa[enemigo->fila][enemigo->columna] = enemigo->bajo;
  enemigo->bajo = destino == JUGADOR ? JUGADOR : destino;
  enemigo->fila = nueva_fila;
  enemigo->columna = nueva_columna;
  mapa[nueva_fila][nueva_columna] = ENEMIGO;

  if (destino == JUGADOR) {
    return 2;
  }

  return 1;
}

int mostrar_game_over(void) {
  while (1) {
    system("cls");
    printf("GAME OVER\n\n");
    printf("[R]etry  [E]xit\n");

    char tecla = tolower(_getch());
    if (tecla == 'r') {
      return 1;
    }
    if (tecla == 'e') {
      return 0;
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

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  WORD origAttr = 0;
  if (GetConsoleScreenBufferInfo(hOut, &csbi)) origAttr = csbi.wAttributes;

  for (char *p = salida; *p != '\0'; ++p) {
    char ch = *p;
    if (ch == '\n') { putchar('\n'); continue; }
    WORD attr = get_color_for_char(ch);
    if (attr) SetConsoleTextAttribute(hOut, attr);
    putchar(ch);
    if (attr) SetConsoleTextAttribute(hOut, origAttr);
  }
  SetConsoleTextAttribute(hOut, origAttr);
}

int main() {
  char mapa1[FILAS][COLUMNAS];
  char mapa2[FILAS][COLUMNAS];
  char mapa3[FILAS][COLUMNAS];
  char (*mapa_actual)[COLUMNAS];
  int fila_jugador;
  int columna_jugador;
  int piso_actual;
  int llaves;
  int monedas_recolectadas;
  int total_monedas;
  char celda_jugador;
  char direccion;
  int num_enemigos1;
  int num_enemigos2;
  int num_enemigos3;
  Enemigo enemigos1[MAX_ENEMIGOS];
  Enemigo enemigos2[MAX_ENEMIGOS];
  Enemigo enemigos3[MAX_ENEMIGOS];
  int retry = 1;

  //reinicia el nivel
  while (retry) {
    mapa_actual = mapa1;
    piso_actual = 1;
    llaves = 0;
    monedas_recolectadas = 0;
    total_monedas = 0;
    celda_jugador = CAMINO;
    num_enemigos1 = num_enemigos2 = num_enemigos3 = 0;

    copiar_mapa(nivel1, mapa1, 1, &fila_jugador, &columna_jugador);
    copiar_mapa(nivel2, mapa2, 0, NULL, NULL);
    copiar_mapa(nivel3, mapa3, 0, NULL, NULL);

    cargar_enemigos(mapa1, enemigos1, &num_enemigos1);
    cargar_enemigos(mapa2, enemigos2, &num_enemigos2);
    cargar_enemigos(mapa3, enemigos3, &num_enemigos3);

    total_monedas = contar_caracter(&mapa1[0][0], FILAS * COLUMNAS, MONEDA) +
                    contar_caracter(&mapa2[0][0], FILAS * COLUMNAS, MONEDA) +
                    contar_caracter(&mapa3[0][0], FILAS * COLUMNAS, MONEDA);

    int game_over = 0;

    do {
      int fila_nueva = fila_jugador;
      int columna_nueva = columna_jugador;
      Enemigo *enemigos = mapa_actual == mapa1 ? enemigos1 :
                         mapa_actual == mapa2 ? enemigos2 : enemigos3;
      int *num_enemigos = mapa_actual == mapa1 ? &num_enemigos1 :
                          mapa_actual == mapa2 ? &num_enemigos2 : &num_enemigos3;

      system("cls");
      imprimir_juego(mapa_actual, fila_jugador, columna_jugador,
                     monedas_recolectadas, llaves, piso_actual);

      direccion = tolower(_getch());

      if (direccion == 'q') {
        game_over = 0;
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

      char destino_jugador = mapa_actual[fila_nueva][columna_nueva];
      if (destino_jugador == ENEMIGO) {
        game_over = 1;
        break;
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

      for (int i = 0; i < *num_enemigos; i++) {
        int resultado = mover_enemigo(mapa_actual, &enemigos[i], fila_jugador,
                                     columna_jugador);
        if (resultado == 2) {
          game_over = 1;
          break;
        }
      }

      if (game_over) {
        break;
      }
    } while (!game_over);

    if (!game_over) {
      break;
    }

    retry = mostrar_game_over();
  }

  return 0;
}
