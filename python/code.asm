

    mov     ax, bx
    bswp    eax

loop:
    inc     dx
    test    cx, 10
    

    jmp     loop
