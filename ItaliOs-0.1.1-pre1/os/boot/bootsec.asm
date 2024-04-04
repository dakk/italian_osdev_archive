;ItaLo italiOs Loader
;funziona solo con processori a 32 bit
; il kernel dal terzo settore ed è lungo KRNL_SIZE settori
;si va poi in modalità protetta e si fa un jump al codice del kernel
; che è un normale binario
%include "kernel_len.inc"

%define DIM_STACK  0x5C2DF

SECTION .bss
  kernel_stack:         resb DIM_STACK
  stack_kernel:



SECTION .text

	org	0x7C00  ;deve essere caricato in ram a partire da questo indirizzo

main:
	xor	ax,ax
	mov	ds,ax
	mov	ss,ax
	mov	sp,0xFFFE


;INT 13 - DISK - READ SECTOR(S) INTO MEMORY
;	AH = 02h
;	AL = number of sectors to read (must be nonzero)
;	CH = low eight bits of cylinder number
;	CL = sector number 1-63 (bits 0-5)
;	     high two bits of cylinder (bits 6-7, hard disk only)
;	DH = head number
;	DL = drive number (bit 7 set for hard disk)
;	ES:BX -> data buffer
;       Return: CF set on error
;      if AH = 11h (corrected ECC error), AL = burst length
;	CF clear if successful
;	AH = status (see #00234)
;	AL = number of sectors transferred (only valid if CF set for some
;	      BIOSes)


	mov	si,KRNL_SIZE
;	mov	si,200
	mov	bx,ax
	mov	ax,0x1000
	mov	es,ax
	mov	cx,3
	xor	dx,dx

.readAgain:
	push ax
	push bx
	mov	ax,0x0E00+'='  
	mov	bx,7
	int	0x10
	pop bx
	pop ax
	
	mov	ax,0x0201
	int	0x13
	jc	short .error

	mov	ax,es
	add	ax,32
	mov	es,ax

	dec	si
	jz	short .readOk

	inc	cx
	cmp	cl,18
	jbe	short .readAgain
	mov	cl,1
	inc	dh
	cmp	dh,2
	jne	short .readAgain
	mov	dh,0
	inc	ch
	jmp	short .readAgain


.error:
;error handler
	mov	ax,0x0E00+'e'   ; se ci sono problemi di letturastampo una e a video e aspetto la pressione di un tasto
	mov	bx,7
	int	0x10
	xor	ax,ax
	int	0x16
	int	0x19

.readOk:

	; spengo il motore del floppy
        mov edx,0x3f2
        mov al,0x0c
        out dx,al

	cli
.wait1:
	in	al, 0x64
	test	al, 2
	jnz	.wait1
	mov	al, 0xD1
	out	0x64, al

.wait2:
	in	al, 0x64
	and	ax, byte 2
	jnz	.wait2
	mov	al, 0xDF
	out	0x60, al

	lgdt	[gdtinfo]  ;carico la gdt

	mov	eax,cr0   ;  metto in eat cr0
	or	al,1      ;imposto il bit per la modalità protetta
	mov	cr0,eax   ;siiiiiiiii, vado in modalità protetta
	jmp dword (flat_code-gdt_table):pmode1; tutto è pronto, e quindi eseguo il kernel

pmode1:
[BITS 32]


	mov	ax, flat_data-gdt_table	; Selector for 4Gb data seg
	;mov	ax,0x10
	mov	ds, ax			; {2} Extend limit for ds
	mov	es, ax			; Extend limit for es
	mov	fs, ax			; fs and...
	mov 	gs, ax			; gs
	mov	ss,ax
;	mov	esp,0xFFFF
	mov	esp,  0x9FFFC

	push dword 2
	popfd

	mov eax, 0x10000

	call eax

	cli
	hlt


;----------------------- GDT -------------------------------------

gdtinfo:

dw	gdtlength
dd	gdt_table


gdt_table:

null_desc	dd	0
		dd	0

flat_code	dw	0xFFFF	;
		dw	0x0000	; Base == 0x00000000
		db	0x00	; Limit == 0xFFFFFFFF
		db	0x9A	; Flag1 == 0x4 == b0100 (G, D/B, reserved, AVL)
		db	0xCF	; Flag2 == 0x9A == b10011010 (P, DPL, S, Type)
		db	0x00	;

flat_data	dw	0xFFFF	;
		dw	0x0000	; Base == 0x00000000
		db	0x00	; Limit == 0xFFFFF
		db	0x92	; Flag1 == 0x4 == b0100 (G, D/B, reserved, AVL)
		db	0xCF	; Flag2 == 0x92 == b10010010 (P, DPL, S, Type)
		db	0x00	;

gdtlength equ $ - gdt_table

;START BOOTSECTOR HEADER
BYTES_PER_SECTOR equ	512
SECTORS_PER_TRACK equ	18
TOTAL_HEADS	equ	2
TOTAL_SECTORS	equ	2880	 ; 1474560/512 for a 1.44meg disk


;END BOOTSECTOR HEADER

bootdrv		db	0

times 510-($-$$) db 0
dw 0xAA55



;----------------------------------------------------------------

