%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
v$i:	resd	1
v$carre:	resd	1

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmain:
	push	ebp
	mov	ebp, esp
	mov	dword [v$i], 0
e0:
	cmp	dword [v$i], 10
	jl	e1
	mov	eax, 0
	jmp	e2
e1:
	mov	eax, 1
e2:
	cmp	eax, 0
	je	e3
	mov	eax, dword [v$i]
	imul	eax, dword [v$i]
	mov	dword [v$carre], eax
	mov	eax, dword [v$carre]
	call	iprintLF
	mov	eax, dword [v$i]
	add	eax, 1
	mov	dword [v$i], eax
	jmp	e0
e3:
	pop	ebp
	ret