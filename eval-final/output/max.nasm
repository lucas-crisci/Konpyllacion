%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmax:
	push	ebp
	mov	ebp, esp
	mov	eax, dword [ebp + 12]
	cmp	eax, dword [ebp + 8]
	jl	e0
	mov	eax, 0
	jmp	e1
e0:
	mov	eax, 1
e1:
	cmp	eax, 0
	je	e2
	mov	eax, dword [ebp + 8]
	mov	dword [ebp + 16], eax
	pop	ebp
	ret
e2:
	mov	eax, dword [ebp + 12]
	mov	dword [ebp + 16], eax
	pop	ebp
	ret
	pop	ebp
	ret
fmain:
	push	ebp
	mov	ebp, esp
	sub	esp, 4	; allocation variable locale v$v_1
	sub	esp, 4	; allocation variable locale v$v_2
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [ebp - 4], eax
	mov	eax, sinput
	call	readline
	mov	eax, sinput
	call	atoi
	mov	dword [ebp - 8], eax
	sub	esp, 4
	push	dword [ebp + 4]
	push	dword [ebp + 0]
	call	max
	pop	eax
	cmp	eax, dword [ebp - 4]
	je	e3
	mov	eax, 0
	jmp	e4
e3:
	mov	eax, 1
e4:
	cmp	eax, 0
	je	e5
	mov	eax, dword [ebp - 4]
	call	iprintLF
	jmp	e6
e5:
	mov	eax, dword [ebp - 8]
	call	iprintLF
e6:
	add	esp, 8
	pop	ebp
	ret
