GLOBAL enablepg
GLOBAL farjump
GLOBAL load_tr


; definitions
%define param1 [ebp + 8]
%define param2 [ebp + 12]


enablepg:
		cli
		mov eax,0
		mov cr3,eax	; Load address of page dir into cr3
		mov ebx,1
		shl ebx,31
		mov ecx,ebx
		mov eax,cr0
		or eax,ebx
		mov cr0,eax	; Set PG bit of CR0
		jmp short flush
flush:
		sti
		ret
		
ALIGN 4
farjump:
	push	ebp
	mov	ebp, esp
	sub	esp, 6		; need 6 bytes for far ptr 
	mov	eax, param1	;  the selector parameter
	mov	word [ebp - 2],ax
	mov	eax, param2	; the offset parameter
	mov	dword [ebp - 6],eax
	jmp	dword far [ebp - 6]
	mov	esp, ebp
	pop	ebp
ret


ALIGN 4
load_tr:
	ltr	[esp + 4]
ret
