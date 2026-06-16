#include "juego.h"

//es la inicialización del juego, se llama al inicio del programa para configurar el estado inicial del juego.

void inicializar_juego(EstadoJuego *estado) {
    estado->fila_jugador = 1;
    estado->columna_jugador = 1;
    estado->ventana_fila_inicio = 0;
    estado->ventana_columna_inicio = 0;
    estado->monedas_recolectadas = 0;
    estado->total_monedas_nivel = 0;
    estado->total_celdas_libres = 0;
    estado->tiene_llave = 0;
    estado->pasos = 0;
    estado->nivel_actual = 1;
}

void cargar_nivel(char *mapa, int nivel, EstadoJuego *estado) {
    // esta función carga el mapa correspondiente del nivel
    // el mapa debe estar en mapa.h
    // aquí se inicializa el estado para el nuevo nivel
    
    estado->monedas_recolectadas = 0;
    estado->tiene_llave = 0;
    estado->pasos = 0;
    estado->fila_jugador = 1;
    estado->columna_jugador = 1;
    estado->ventana_fila_inicio = 0;
    estado->ventana_columna_inicio = 0;
    
    // calcular total de monedas del nivel usando NASM
    estado->total_monedas_nivel = contar_caracter(mapa, MAPA_FILAS * MAPA_COLUMNAS, MONEDA);
    
    // cntar celdas libres del nivel usando NASM
    estado->total_celdas_libres = contar_celdas_libres(mapa, MAPA_FILAS * MAPA_COLUMNAS);
}
//pantalla y la visualizacion

void imprimir_ventana_visible(char *mapa, EstadoJuego *estado) {
    system("cls"); // Limpia la pantalla (Windows)
    

    //mostrar información del nivel en la parte superior
    mostrar_hud(estado);
    
    printf("\n");
    
    //imprimir la ventana visible del mapa
    int inicio_fila = estado->ventana_fila_inicio;
    int inicio_columna = estado->ventana_columna_inicio;
    
    for (int i = 0; i < VENTANA_FILAS; i++) {
        for (int j = 0; j < VENTANA_COLUMNAS; j++) {
            int fila_mapa = inicio_fila + i;
            int columna_mapa = inicio_columna + j;
            
            // Validar límites
            if (fila_mapa >= MAPA_FILAS || columna_mapa >= MAPA_COLUMNAS) {
                printf("#"); // Mostrar pared fuera de límites
            } else {
                int indice = fila_mapa * MAPA_COLUMNAS + columna_mapa;
                printf("%c", mapa[indice]);
            }
        }
        printf("\n");
    }
}

void actualizar_ventana_visible(EstadoJuego *estado) {
    // calcular la posición inicial de la ventana
    // la idea es que el jugador esté centrado en la ventana (aproximadamente)
    
    int centro_ventana_fila = VENTANA_FILAS / 2;
    int centro_ventana_columna = VENTANA_COLUMNAS / 2;
    
    //calcular la esquina superior izquierda de la ventana
    int nueva_fila = estado->fila_jugador - centro_ventana_fila;
    int nueva_columna = estado->columna_jugador - centro_ventana_columna;
    

    if (nueva_fila < 0) {
        nueva_fila = 0;
    }
    if (nueva_columna < 0) {
        nueva_columna = 0;
    }
    
    //validar límites inferior y derecho
    if (nueva_fila + VENTANA_FILAS > MAPA_FILAS) {
        nueva_fila = MAPA_FILAS - VENTANA_FILAS;
    }
    if (nueva_columna + VENTANA_COLUMNAS > MAPA_COLUMNAS) {
        nueva_columna = MAPA_COLUMNAS - VENTANA_COLUMNAS;
    }
    
    estado->ventana_fila_inicio = nueva_fila;
    estado->ventana_columna_inicio = nueva_columna;
}

void mostrar_hud(EstadoJuego *estado) {
    printf("==================================\n");
    printf("Nivel: %d\n", estado->nivel_actual);
    printf("Llave: %s\n", estado->tiene_llave ? "Si" : "No");
    printf("Pasos: %d\n", estado->pasos);
    printf("Monedas: %d / %d\n", estado->monedas_recolectadas, estado->total_monedas_nivel);
    printf("==================================\n");
}

void mostrar_resumen_nivel(EstadoJuego *estado) {
    printf("\n");
    printf("==================================\n");
    printf("Nivel %d completado\n", estado->nivel_actual);
    printf("Monedas recolectadas: %d / %d\n", estado->monedas_recolectadas, estado->total_monedas_nivel);
    printf("Pasos realizados: %d\n", estado->pasos);
    printf("==================================\n");
    printf("Presione una tecla para continuar...\n");
    getchar();
}

void mostrar_resumen_final(EstadoJuego *estado, long long puntaje_final) {
    system("cls");
    printf("\n");
    printf("==================================\n");
    printf("Juego completado\n");
    printf("Monedas totales recolectadas: [IMPLEMENTAR]\n");
    printf("Pasos totales: [IMPLEMENTAR]\n");
    printf("Niveles completados: %d\n", estado->nivel_actual);
    printf("Puntaje final: %lld\n", puntaje_final);
    printf("==================================\n");
}

//movimiento y entrada

char capturar_entrada() {
    // implementación simple de captura de entrada
    char tecla;
    scanf("%c", &tecla);
    return tecla;
}

void procesar_movimiento(char *mapa, char tecla, EstadoJuego *estado) {
    int nueva_fila = estado->fila_jugador;
    int nueva_columna = estado->columna_jugador;
    
    //determinar la nueva posición según la tecla presionada
    switch (tecla) {
        case 'W':
        case 'w':
            nueva_fila--;
            break;
        case 'A':
        case 'a':
            nueva_columna--;
            break;
        case 'S':
        case 's':
            nueva_fila++;
            break;
        case 'D':
        case 'd':
            nueva_columna++;
            break;
        default:
            return; // Tecla no válida
    }
    
    //intentar mover el jugador
    mover_jugador(mapa, nueva_fila, nueva_columna, estado);
}

int mover_jugador(char *mapa, int nueva_fila, int nueva_columna, EstadoJuego *estado) {
    //validar limites del mapa
    if (nueva_fila < 0 || nueva_fila >= MAPA_FILAS || 
        nueva_columna < 0 || nueva_columna >= MAPA_COLUMNAS) {
        return 0; // Movimiento bloqueado
    }
    
    //usar función NASM para validar el movimiento
    int movimiento_valido = validar_movimiento(mapa, MAPA_COLUMNAS, nueva_fila, nueva_columna);
    
    if (movimiento_valido) {
        //reemplazar la posición anterior del jugador con un camino
        int indice_anterior = estado->fila_jugador * MAPA_COLUMNAS + estado->columna_jugador;
        mapa[indice_anterior] = CAMINO;
        
        //actualizar posición del jugador
        estado->fila_jugador = nueva_fila;
        estado->columna_jugador = nueva_columna;
        
        //actualizar la ventana visible
        actualizar_ventana_visible(estado);
        
        // incrementar contador de pasos
        estado->pasos++;
        
        //colocar el carácter del jugador
        int indice_nuevo = estado->fila_jugador * MAPA_COLUMNAS + estado->columna_jugador;
        mapa[indice_nuevo] = JUGADOR;
        
        return 1; // Movimiento exitoso
    }
    
    return 0; // Movimiento bloqueado (pared)
}

//verificacion de objetos

void verificar_moneda(char *mapa, EstadoJuego *estado) {
    // Usar función NASM para detectar si hay moneda
    int hay_moneda = detectar_objeto(mapa, MAPA_COLUMNAS, estado->fila_jugador, estado->columna_jugador,MONEDA);
    
    if (hay_moneda) {
        estado->monedas_recolectadas++;
        //la moneda ya se reemplazó con el jugador
    }
}

void verificar_llave(char *mapa, EstadoJuego *estado) {
    //usar función NASM para detectar si hay llave
    int hay_llave = detectar_objeto(mapa, MAPA_COLUMNAS, estado->fila_jugador,estado->columna_jugador, LLAVE);
    
    if (hay_llave) {
        estado->tiene_llave = 1;
        // Reemplazar la llave con el jugador
        int indice = estado->fila_jugador * MAPA_COLUMNAS + estado->columna_jugador;
        mapa[indice] = JUGADOR;
    }
}

int verificar_puerta(EstadoJuego *estado) {
    // verificar si el jugador está en la puerta
    if (estado->tiene_llave) {
        return 1; // Puede cruzar la puerta
    }
    return 0; // No puede cruzar
}

int verificar_salida(char *mapa, EstadoJuego *estado) {
    // usar función NASM para detectar si hay salida
    int hay_salida = detectar_objeto(mapa, MAPA_COLUMNAS, estado->fila_jugador,estado->columna_jugador,SALIDA);
    
    return hay_salida;
}

//se ejcuta el nivel

int ejecutar_nivel(char *mapa, EstadoJuego *estado) {
    char tecla;
    
    while (1) {
        //mostrar el mapa
        imprimir_ventana_visible(mapa, estado);
        
        //capturar entrada 
        printf("\nTecla (W/A/S/D/Q): ");
        tecla = capturar_entrada();
        
        //si es que quiere salir
        if (tecla == 'Q' || tecla == 'q') {
            return -1; 
        }
        
        //procesar movimiento
        procesar_movimiento(mapa, tecla, estado);
        
        //verificar si recolectó moneda
        verificar_moneda(mapa, estado);
        
        // verificar si recolectó llave
        verificar_llave(mapa, estado);
        
        // verificar si llegó a la salida
        if (verificar_salida(mapa, estado)) {
            printf("\nSalida encontrada! Nivel completado.\n");
            return 1; 
        }
    }
    
    return 0;
}
