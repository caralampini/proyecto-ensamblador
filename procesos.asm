bits 64
default rel

global generar_ventana
global contar_caracter
global validar_movimiento
global detectar_objeto

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
