.code

FastCopy PROC
    mov     rax, rcx

    test    rcx, rcx
    jz      done
    test    rdx, rdx
    jz      done
    test    r8, r8
    jz      done

    cmp     r8, 16
    jb      copy_scalar

    cmp     r8, 64
    jb      copy_xmm_16_63

    cmp     r8, 256
    jb      copy_xmm_64_255

    jmp     copy_rep_movsb

copy_scalar:
scalar_loop:
    mov     r9b, byte ptr [rdx]
    mov     byte ptr [rcx], r9b
    inc     rdx
    inc     rcx
    dec     r8
    jnz     scalar_loop
    ret

copy_xmm_16_63:
    cmp     r8, 32
    jbe     xmm_16_32

    cmp     r8, 48
    jbe     xmm_33_48

    movdqu  xmm0, xmmword ptr [rdx]
    movdqu  xmm1, xmmword ptr [rdx + 16]
    movdqu  xmm2, xmmword ptr [rdx + 32]
    movdqu  xmm3, xmmword ptr [rdx + r8 - 16]

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm1
    movdqu  xmmword ptr [rcx + 32], xmm2
    movdqu  xmmword ptr [rcx + r8 - 16], xmm3
    ret

xmm_33_48:
    movdqu  xmm0, xmmword ptr [rdx]
    movdqu  xmm1, xmmword ptr [rdx + 16]
    movdqu  xmm2, xmmword ptr [rdx + r8 - 16]

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm1
    movdqu  xmmword ptr [rcx + r8 - 16], xmm2
    ret

xmm_16_32:
    movdqu  xmm0, xmmword ptr [rdx]
    movdqu  xmm1, xmmword ptr [rdx + r8 - 16]

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + r8 - 16], xmm1
    ret

copy_xmm_64_255:
xmm_loop:
    cmp     r8, 64
    jb      xmm_tail

    movdqu  xmm0, xmmword ptr [rdx]
    movdqu  xmm1, xmmword ptr [rdx + 16]
    movdqu  xmm2, xmmword ptr [rdx + 32]
    movdqu  xmm3, xmmword ptr [rdx + 48]

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm1
    movdqu  xmmword ptr [rcx + 32], xmm2
    movdqu  xmmword ptr [rcx + 48], xmm3

    add     rdx, 64
    add     rcx, 64
    sub     r8, 64
    jmp     xmm_loop

xmm_tail:
    test    r8, r8
    jz      done
    jmp     copy_scalar

copy_rep_movsb:
    push    rdi
    push    rsi

    mov     rdi, rcx
    mov     rsi, rdx
    mov     rcx, r8
    cld
    rep movsb

    pop     rsi
    pop     rdi
    ret

done:
    ret
FastCopy ENDP


FastMove PROC
    mov     rax, rcx

    test    rcx, rcx
    jz      move_done
    test    rdx, rdx
    jz      move_done
    test    r8, r8
    jz      move_done

    cmp     rcx, rdx
    je      move_done

    cmp     rcx, rdx
    jb      move_forward

    lea     r9, [rdx + r8]
    cmp     rcx, r9
    jae     move_forward

    jmp     move_backward

move_forward:
    jmp     FastCopy

move_backward:
    std

    push    rdi
    push    rsi

    lea     rdi, [rcx + r8 - 1]
    lea     rsi, [rdx + r8 - 1]
    mov     rcx, r8
    rep movsb

    cld

    pop     rsi
    pop     rdi
    ret

move_done:
    ret
FastMove ENDP


FastSet PROC
    mov     rax, rcx

    test    rcx, rcx
    jz      set_done
    test    r8, r8
    jz      set_done

    cmp     r8, 16
    jb      set_scalar

    cmp     r8, 64
    jb      set_xmm_16_63

    cmp     r8, 256
    jb      set_xmm_64_255

    jmp     set_rep_stosb

set_scalar:
scalar_set_loop:
    mov     byte ptr [rcx], dl
    inc     rcx
    dec     r8
    jnz     scalar_set_loop
    ret

set_xmm_16_63:
    movzx   r9d, dl
    imul    r9d, 01010101h
    movd    xmm0, r9d
    pshufd  xmm0, xmm0, 0

    cmp     r8, 32
    jbe     set_xmm_16_32

    cmp     r8, 48
    jbe     set_xmm_33_48

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm0
    movdqu  xmmword ptr [rcx + 32], xmm0
    movdqu  xmmword ptr [rcx + r8 - 16], xmm0
    ret

set_xmm_33_48:
    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm0
    movdqu  xmmword ptr [rcx + r8 - 16], xmm0
    ret

set_xmm_16_32:
    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + r8 - 16], xmm0
    ret

set_xmm_64_255:
    movzx   r9d, dl
    imul    r9d, 01010101h
    movd    xmm0, r9d
    pshufd  xmm0, xmm0, 0

set_xmm_loop:
    cmp     r8, 64
    jb      set_xmm_tail

    movdqu  xmmword ptr [rcx], xmm0
    movdqu  xmmword ptr [rcx + 16], xmm0
    movdqu  xmmword ptr [rcx + 32], xmm0
    movdqu  xmmword ptr [rcx + 48], xmm0

    add     rcx, 64
    sub     r8, 64
    jmp     set_xmm_loop

set_xmm_tail:
    test    r8, r8
    jz      set_done
    jmp     set_scalar

set_rep_stosb:
    push    rdi

    mov     rdi, rcx
    mov     al, dl
    mov     rcx, r8
    cld
    rep stosb

    pop     rdi
    ret

set_done:
    ret
FastSet ENDP


FastCompare PROC
    test    r8, r8
    jz      compare_equal

    test    rcx, rcx
    jz      compare_left_null
    test    rdx, rdx
    jz      compare_right_null

    cmp     rcx, rdx
    je      compare_equal

    cmp     r8, 16
    jb      compare_scalar

    jmp     compare_xmm

compare_left_null:
    test    rdx, rdx
    jz      compare_equal
    mov     eax, -1
    ret

compare_right_null:
    mov     eax, 1
    ret

compare_scalar:
    movzx   eax, byte ptr [rcx]
    movzx   r9d, byte ptr [rdx]
    cmp     al, r9b
    jne     compare_diff_eax_r9d

    inc     rcx
    inc     rdx
    dec     r8
    jnz     compare_scalar

compare_equal:
    xor     eax, eax
    ret

compare_xmm:
    cmp     r8, 16
    jb      compare_equal

    movdqu  xmm0, xmmword ptr [rcx]
    movdqu  xmm1, xmmword ptr [rdx]
    pcmpeqb xmm0, xmm1
    pmovmskb eax, xmm0

    cmp     eax, 0FFFFh
    jne     compare_xmm_mismatch

    add     rcx, 16
    add     rdx, 16
    sub     r8, 16
    jz      compare_equal
    jmp     compare_xmm

compare_xmm_mismatch:
    not     eax
    and     eax, 0FFFFh
    bsf     eax, eax

    movzx   r10d, byte ptr [rcx + rax]
    movzx   r11d, byte ptr [rdx + rax]
    mov     eax, r10d
    sub     eax, r11d
    ret

compare_diff_eax_r9d:
    sub     eax, r9d
    ret
FastCompare ENDP

END