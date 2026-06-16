#ifndef JUEGO_H
#define JUEGO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//constantes
#define MAPA_FILAS 60
#define MAPA_COLUMNAS 60
#define VENTANA_FILAS 20
#define VENTANA_COLUMNAS 20
#define TOTAL_NIVELES 3

//los caracteres del mapa
#define PARED '#'
#define CAMINO '.'
#define JUGADOR 'P'
#define MONEDA 'M'
#define LLAVE 'K'
#define PUERTA 'D'
#define SALIDA 'E'

#define ENEMIGO 'X'

//estructuta del juego
typedef struct {
    int fila_jugador;
    int columna_jugador;
    int ventana_fila_inicio;
    int ventana_columna_inicio;
    int monedas_recolectadas;
    int total_monedas_nivel;
    int total_celdas_libres;
    int tiene_llave;
    int pasos;
    int nivel_actual;
} EstadoJuego;

//estructura del resumen
typedef struct {
    int nivel;
    int monedas_recolectadas;
    int total_monedas;
    int pasos;
} ResumenNivel;

//funciones para NASM

extern int contar_caracter(char *mapa, int total_celdas, char caracter);
extern int validar_movimiento(char *mapa, int columnas, int fila, int columna, int tiene_llave);
extern long long calcular_puntaje(int monedas, int pasos, int niveles);
extern int detectar_objeto(char *mapa, int columnas, int fila, int columna, char objeto);
extern int contar_celdas_libres(char *mapa, int total_celdas);


//funciones implementadas en c
void inicializar_juego(EstadoJuego *estado);
void cargar_nivel(char *mapa, int nivel, EstadoJuego *estado);
void imprimir_ventana_visible(char *mapa, EstadoJuego *estado);
void actualizar_ventana_visible(EstadoJuego *estado);
void procesar_movimiento(char *mapa, char tecla, EstadoJuego *estado);
int mover_jugador(char *mapa, int nueva_fila, int nueva_columna, EstadoJuego *estado);
void verificar_moneda(char *mapa, EstadoJuego *estado);
void verificar_llave(char *mapa, EstadoJuego *estado);
int verificar_puerta(EstadoJuego *estado);
int verificar_salida(char *mapa, EstadoJuego *estado);
void mostrar_hud(EstadoJuego *estado);
void mostrar_resumen_nivel(EstadoJuego *estado);
void mostrar_resumen_final(EstadoJuego *estado, long long puntaje_final);
char capturar_entrada();
int ejecutar_nivel(char *mapa, EstadoJuego *estado);

#endif 
