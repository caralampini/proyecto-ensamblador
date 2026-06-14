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
    ;stack entero fila
    ;stack entero columna
    mov r10d, [rsp + 40]
    mov r11d, [rsp + 48]
    mov rax, rcx

    cmp rcx, rdx
    je .mapa1
    cmp rcx, r8
    je .mapa2
    cmp rcx, r9
    je .mapa3
    ret

.mapa1:
    cmp r10d, 0
    jne .sin_cambio
    cmp r11d, 2
    jne .sin_cambio
    mov rax, r8
    ret

.mapa2:
    cmp r10d, 0
    jne .mapa2_atico
    cmp r11d, 2
    jne .mapa2_atico
    mov rax, rdx
    ret

.mapa2_atico:
    cmp r10d, 59
    jne .sin_cambio
    cmp r11d, 7
    jne .sin_cambio
    mov rax, r9
    ret

.mapa3:
    cmp r10d, 0
    jne .sin_cambio
    cmp r11d, 56
    jne .sin_cambio
    mov rax, r8
    ret

.sin_cambio:
    ret
