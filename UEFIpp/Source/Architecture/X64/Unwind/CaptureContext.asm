; void CaptureUnwindContext(Context* Context);
; RCX = Context
;
; Captures the state of the caller at the instruction immediately after the
; call to this routine. The x64 ABI guarantees that the nonvolatile registers
; observed here are the same values owned by the caller.

.code

CaptureUnwindContext PROC
	mov     rax, [rsp]
	mov     [rcx + 00h], rax

	lea     rax, [rsp + 8]
	mov     [rcx + 08h], rax

	mov     [rcx + 10h], rbx
	mov     [rcx + 18h], rbp
	mov     [rcx + 20h], rsi
	mov     [rcx + 28h], rdi
	mov     [rcx + 30h], r12
	mov     [rcx + 38h], r13
	mov     [rcx + 40h], r14
	mov     [rcx + 48h], r15
	ret
CaptureUnwindContext ENDP

END