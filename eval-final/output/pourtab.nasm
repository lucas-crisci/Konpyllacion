%include	'io.asm'

section	.bss
sinput:	resb	255	;reserve a 255 byte space in memory for the users input string
vv:	resd	20
vi:	resd	1

section	.text
global _start
_start:
	call	fmain
	mov	eax, 1
	int	0x80
fmain:
	push	ebp
	mov	ebp, esp
	mov	dword [vv+0], 5
	pop	ebp
	ret
