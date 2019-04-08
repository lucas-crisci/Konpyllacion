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
finitialiser:
	push	ebp
	mov	ebp, esp
	mov	dword [v$tab+0], 8
	mov	dword [v$tab+4], 6
	mov	dword [v$tab+8], 9
	mov	dword [v$tab+12], 9
	mov	dword [v$tab+16], 4
	mov	dword [v$tab+20], 2
	mov	dword [v$tab+24], 3
	mov	dword [v$tab+28], 1
	mov	dword [v$tab+32], 4
	mov	dword [v$tab+36], 5
	pop	ebp
	ret
fafficher:
	push	ebp
	mov	ebp, esp
	sub	esp, 4	; allocation variable locale v$i
	mov	dword [ebp - 4], 0
e0:
	mov	eax, dword [ebp - 4]
	cmp	eax, dword [ebp + 8]
	jl	e1
	mov	eax, 0
	jmp	e2
e1:
	mov	eax, 1
e2:
	cmp	eax, 0
	je	e3
	mov	eax, dword [ebp - 4]
	add	eax, eax
	add	eax, eax
	mov	eax, dword [v$tab+eax]
	call	iprintLF
	mov	eax, dword [ebp - 4]
	add	eax, 1
	mov	dword [ebp - 4], eax
	jmp	e0
e3:
	mov	eax, 0
	call	iprintLF
	add	esp, 4
	pop	ebp
	ret
fechanger:
	push	ebp
	mov	ebp, esp
	sub	esp, 4	; allocation variable locale v$temp
	mov	eax, dword [ebp + 8]
	add	eax, eax
	add	eax, eax
	mov	ebx, dword [v$tab+eax]
	mov	dword [ebp - 4], ebx
	mov	eax, dword [ebp + 8]
	mov	ebx, dword [ebp + 12]
	add	ebx, ebx
	add	ebx, ebx
	mov	ecx, dword [v$tab+ebx]
	add	eax, eax
	add	eax, eax
	mov	dword [v$tab+eax], ecx
	mov	eax, dword [ebp + 12]
	mov	ebx, dword [ebp - 4]
	add	eax, eax
	add	eax, eax
	mov	dword [v$tab+eax], ebx
	add	esp, 4
	pop	ebp
	ret
ftrier:
	push	ebp
	mov	ebp, esp
	sub	esp, 4	; allocation variable locale v$echange
	sub	esp, 4	; allocation variable locale v$j
	sub	esp, 4	; allocation variable locale v$m
	mov	eax, dword [ebp + 8]
	mov	dword [ebp - 12], eax
	mov	dword [ebp - 4], 1
e4:
	cmp	dword [ebp - 4], 1
	je	e5
	mov	eax, 0
	jmp	e6
e5:
	mov	eax, 1
e6:
	cmp	eax, 0
	je	e7
	mov	dword [ebp - 4], 0
	mov	dword [ebp - 8], 0
e8:
	mov	eax, dword [ebp - 12]
	sub	eax, 1
	cmp	dword [ebp - 8], eax
	jl	e9
	mov	eax, 0
	jmp	e10
e9:
	mov	eax, 1
e10:
	cmp	eax, 0
	je	e11
	mov	eax, dword [ebp - 8]
	add	eax, 1
	mov	ebx, dword [ebp - 8]
	add	eax, eax
	add	eax, eax
	mov	ecx, dword [v$tab+eax]
	add	ebx, ebx
	add	ebx, ebx
	cmp	ecx, dword [v$tab+ebx]
	jl	e12
	mov	eax, 0
	jmp	e13
e12:
	mov	eax, 1
e13:
	cmp	eax, 0
	je	e14
	sub	esp, 4
	push	dword [ebp + 4]
	mov	eax, dword [ebp - 8]
	add	eax, 1
	push	eax
	call	fechanger
	add	esp, 8
	add	esp, 4
	mov	dword [ebp - 4], 1
e14:
	mov	eax, dword [ebp - 8]
	add	eax, 1
	mov	dword [ebp - 8], eax
	jmp	e8
e11:
	mov	eax, dword [ebp - 12]
	sub	eax, 1
	mov	dword [ebp - 12], eax
	jmp	e4
e7:
	add	esp, 12
	pop	ebp
	ret
fmain:
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	call	finitialiser
	add	esp, 4
	sub	esp, 4
	push	10
	call	fafficher
	add	esp, 4
	add	esp, 4
	sub	esp, 4
	push	10
	call	ftrier
	add	esp, 4
	add	esp, 4
	sub	esp, 4
	push	10
	call	fafficher
	add	esp, 4
	add	esp, 4
	pop	ebp
	ret
