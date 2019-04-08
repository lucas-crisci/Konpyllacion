%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
v$tab:	resd	10

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmain:
	push	ebp
	mov	ebp, esp
	mov	dword [v$tab+16], 4
	mov	eax, 4
	add	eax, eax
	add	eax, eax
	mov	eax, dword [v$tab+eax]
	call	iprintLF
	pop	ebp
	ret
