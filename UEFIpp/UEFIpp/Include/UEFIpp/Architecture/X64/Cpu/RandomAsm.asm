.code

UEFIpp_X64_RdRand16 proc
    rdrand ax
    setc dl
    mov [rcx], ax
    movzx eax, dl
    ret
UEFIpp_X64_RdRand16 endp

UEFIpp_X64_RdRand32 proc
    rdrand eax
    setc dl
    mov [rcx], eax
    movzx eax, dl
    ret
UEFIpp_X64_RdRand32 endp

UEFIpp_X64_RdRand64 proc
    rdrand rax
    setc dl
    mov [rcx], rax
    movzx eax, dl
    ret
UEFIpp_X64_RdRand64 endp

UEFIpp_X64_RdSeed16 proc
    rdseed ax
    setc dl
    mov [rcx], ax
    movzx eax, dl
    ret
UEFIpp_X64_RdSeed16 endp

UEFIpp_X64_RdSeed32 proc
    rdseed eax
    setc dl
    mov [rcx], eax
    movzx eax, dl
    ret
UEFIpp_X64_RdSeed32 endp

UEFIpp_X64_RdSeed64 proc
    rdseed rax
    setc dl
    mov [rcx], rax
    movzx eax, dl
    ret
UEFIpp_X64_RdSeed64 endp

end