bits 64
default rel

global generar_ventana
global contar_caracter
global validar_movimiento
global detectar_objeto
global cambiar_mapa

section .text

generar_ventana:
    cmp dword [rsp + 48], 0
    jle .salida_vacia

    xor r10d, r10d

.fila_loop:
    xor r11d, r11d

.columna_loop:
    mov eax, r9d
    add eax, r10d
    imul eax, r8d
    add eax, [rsp + 40]
    add eax, r11d
    movsxd rax, eax

    mov al, [rcx + rax]
    mov [rdx], al
    inc rdx

    inc r11d
    cmp r11d, [rsp + 48]
    jl .columna_loop

    mov byte [rdx], 10
    inc rdx

    inc r10d
    cmp r10d, [rsp + 48]
    jl .fila_loop

    mov byte [rdx], 0
    ret

.salida_vacia:
    mov byte [rdx], 0
    ret

contar_caracter:
    xor eax, eax
    test edx, edx
    jle .fin_contar

.loop_contar:
    cmp byte [rcx], r8b
    jne .siguiente_contar
    inc eax

.siguiente_contar:
    inc rcx
    dec edx
    jnz .loop_contar

.fin_contar:
    ret

validar_movimiento:
    mov eax, r8d
    imul eax, edx
    add eax, r9d
    movsxd rax, eax

    mov r10b, [rcx + rax]

    cmp r10b, '#'
    je .mov_bloqueado

    cmp r10b, 'D'
    jne .mov_valido

    cmp dword [rsp + 40], 0
    je .mov_bloqueado

.mov_valido:
    mov eax, 1
    ret

.mov_bloqueado:
    xor eax, eax
    ret

detectar_objeto:
    mov eax, r8d
    imul eax, edx
    add eax, r9d
    movsxd rax, eax

    mov r10b, [rsp + 40]
    cmp [rcx + rax], r10b
    jne .no_encontrado

    mov eax, 1
    ret

.no_encontrado:
    xor eax, eax
    ret

cambiar_mapa:
    ;rcx puntero mapa actual
    ;rdx mapa 1
    ;r8  mapa 2
    ;r9  mapa 3
    ;stack entero x
    ;stack entero y
    mov r10,[rsp+40]
    mov r11,[rsp+48]

    cmp rcx, rdx
    je .escaleras_mapa1
    cmp rcx, r8
    je .escaleras_mapa2
    cmp rcx,r9
    je .escaleras_mapa3

    .escaleras_mapa1:

    cmp r10,1
    jne .otrasescaleras
    cmp r11,1
    jne .otrasescaleras
    mov rcx,r8
    ret

    .otrasescaleras:
    cmp r10,57
    jne .noesnadawey
    cmp r10,1
    mov rcx, r9
    ret

    .escaleras_mapa2:

    cmp r10,1
    jne .noesnadawey
    cmp r11,1
    jne .noesnadawey
    mov rcx,r8
    ret

    .escaleras_mapa3:

    cmp r10,57
    jne .noesnadawey
    cmp r11,1
    jne .noesnadawey
    mov rcx,r8
    ret

    .noesnadawey:
    ret

contar_celdas_libres:

    push r12
    push r13
    push r14
    cmp rcx, 0
    je .matriz_limpia
    
    mov r10d, 0;fila
    mov r11d, 0;columnas
    mov r12d, 0;contador
    mov r13b, 0;lector
    mov r14b, 0;desplazamiento

    .loop_filas:
    mov r11d, 0
    .loop_columnas:

    mov r14d, r10d
    imul r14d, r8d
    add r14d, r11d

    mov  r13b, [rcx+r14d]
    cmp r13b, '.'
    jne .continue
    inc r12d

    .continue:

    
    inc r11d
    cmp r11d, r8d
    jne .loop_columnas

    
    inc r10d
    cmp r10d, edx
    jne .loop_filas

    mov eax, r12d
    pop r14
    pop r13
    pop r12

    ret

    .matriz_limpia:
    mov eax,0
    pop r14
    pop r13
    pop r12
   
    ret
