.code

; uint8_t AsmRdRand64(uint64_t* Value)
AsmRdRand64 PROC
    rdrand rax
    jnc fail
    mov [rcx], rax
    mov al, 1
    ret
fail:
    xor al, al
    ret
AsmRdRand64 ENDP

; uint8_t AsmRdSeed64(uint64_t* Value)
AsmRdSeed64 PROC
    rdseed rax
    jnc fail2
    mov [rcx], rax
    mov al, 1
    ret
fail2:
    xor al, al
    ret
AsmRdSeed64 ENDP

END