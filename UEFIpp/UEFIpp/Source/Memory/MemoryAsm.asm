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

    cmp     r8, 4096
    jb      copy_rep_movsb

    cmp     r8, 1048576
    jae     copy_avx2_nt

    jmp     copy_avx2

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

copy_avx2:
avx2_loop:
    cmp     r8, 128
    jb      avx2_tail

    vmovdqu ymm0, ymmword ptr [rdx]
    vmovdqu ymm1, ymmword ptr [rdx + 32]
    vmovdqu ymm2, ymmword ptr [rdx + 64]
    vmovdqu ymm3, ymmword ptr [rdx + 96]

    vmovdqu ymmword ptr [rcx], ymm0
    vmovdqu ymmword ptr [rcx + 32], ymm1
    vmovdqu ymmword ptr [rcx + 64], ymm2
    vmovdqu ymmword ptr [rcx + 96], ymm3

    add     rdx, 128
    add     rcx, 128
    sub     r8, 128
    jmp     avx2_loop

avx2_tail:
    vzeroupper
    test    r8, r8
    jz      done
    jmp     copy_rep_movsb

copy_avx2_nt:
avx2_nt_loop:
    cmp     r8, 128
    jb      avx2_nt_tail

    vmovdqu     ymm0, ymmword ptr [rdx]
    vmovdqu     ymm1, ymmword ptr [rdx + 32]
    vmovdqu     ymm2, ymmword ptr [rdx + 64]
    vmovdqu     ymm3, ymmword ptr [rdx + 96]

    vmovntdq    ymmword ptr [rcx], ymm0
    vmovntdq    ymmword ptr [rcx + 32], ymm1
    vmovntdq    ymmword ptr [rcx + 64], ymm2
    vmovntdq    ymmword ptr [rcx + 96], ymm3

    add     rdx, 128
    add     rcx, 128
    sub     r8, 128
    jmp     avx2_nt_loop

avx2_nt_tail:
    sfence
    vzeroupper

    test    r8, r8
    jz      done
    jmp     copy_rep_movsb

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

    cmp     r8, 4096
    jb      set_rep_stosb

    cmp     r8, 1048576
    jae     set_avx2_nt

    jmp     set_avx2

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

set_avx2:
    movzx   r9d, dl
    imul    r9d, 01010101h
    vmovd   xmm0, r9d
    vpshufd xmm0, xmm0, 0
    vinserti128 ymm0, ymm0, xmm0, 1

set_avx2_loop:
    cmp     r8, 128
    jb      set_avx2_tail

    vmovdqu ymmword ptr [rcx], ymm0
    vmovdqu ymmword ptr [rcx + 32], ymm0
    vmovdqu ymmword ptr [rcx + 64], ymm0
    vmovdqu ymmword ptr [rcx + 96], ymm0

    add     rcx, 128
    sub     r8, 128
    jmp     set_avx2_loop

set_avx2_tail:
    vzeroupper
    test    r8, r8
    jz      set_done
    jmp     set_rep_stosb

set_avx2_nt:
    movzx   r9d, dl
    imul    r9d, 01010101h
    vmovd   xmm0, r9d
    vpshufd xmm0, xmm0, 0
    vinserti128 ymm0, ymm0, xmm0, 1

set_avx2_nt_loop:
    cmp     r8, 128
    jb      set_avx2_nt_tail

    vmovntdq ymmword ptr [rcx], ymm0
    vmovntdq ymmword ptr [rcx + 32], ymm0
    vmovntdq ymmword ptr [rcx + 64], ymm0
    vmovntdq ymmword ptr [rcx + 96], ymm0

    add     rcx, 128
    sub     r8, 128
    jmp     set_avx2_nt_loop

set_avx2_nt_tail:
    sfence
    vzeroupper

    test    r8, r8
    jz      set_done
    jmp     set_rep_stosb

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

    cmp     r8, 256
    jb      compare_xmm

    jmp     compare_avx2

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
    jb      compare_scalar

    movdqu  xmm0, xmmword ptr [rcx]
    movdqu  xmm1, xmmword ptr [rdx]
    pcmpeqb xmm0, xmm1
    pmovmskb eax, xmm0

    cmp     eax, 0FFFFh
    jne     compare_xmm_mismatch

    add     rcx, 16
    add     rdx, 16
    sub     r8, 16
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

compare_avx2:
    cmp     r8, 32
    jb      compare_xmm

    vmovdqu ymm0, ymmword ptr [rcx]
    vpcmpeqb ymm0, ymm0, ymmword ptr [rdx]
    vpmovmskb eax, ymm0

    cmp     eax, 0FFFFFFFFh
    jne     compare_avx2_mismatch

    add     rcx, 32
    add     rdx, 32
    sub     r8, 32
    jmp     compare_avx2

compare_avx2_mismatch:
    not     eax
    bsf     eax, eax

    movzx   r10d, byte ptr [rcx + rax]
    movzx   r11d, byte ptr [rdx + rax]
    mov     eax, r10d
    sub     eax, r11d

    vzeroupper
    ret

compare_diff_eax_r9d:
    sub     eax, r9d
    ret
FastCompare ENDP

END