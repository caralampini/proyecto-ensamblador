bits 64
default rel

global generar_ventana, intercambiar

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
    mov eax, [rcx + rax * 4]

    cmp eax, 0
    je .valor_vacio
    cmp eax, 1
    je .valor_muro
    cmp eax, 2
    je .valor_jugador

    mov al, '?'
    jmp .escribir

    .valor_vacio:
    mov al, '.'
    jmp .escribir

    .valor_muro:
    mov al, '#'
    jmp .escribir

    .valor_jugador:
    mov al, '@'

    .escribir:
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

intercambiar:
    mov eax, r8d
    imul eax, edx
    add eax, r9d

    movsxd rax, eax
    lea r10, [rcx + rax * 4]

    mov r11d, [rsp + 40]
    mov eax, r11d
    imul eax, edx
    mov r11d, [rsp + 48]
    add eax, r11d

    movsxd rax, eax
    lea r11, [rcx + rax * 4]

    mov eax, [r10]
    mov r8d, [r11]

    mov [r10], r8d
    mov [r11], eax
    ret
