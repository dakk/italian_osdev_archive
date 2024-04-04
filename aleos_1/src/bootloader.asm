; AleOS bootloader
; (by ItaliOS)

KRNL_SIZE	equ	 50
SECTION .text

	org	0x7C00  
	
main:
	xor	ax,ax	
	mov	ds,ax	
	mov	ss,ax	
	mov	sp,0xFFFE


	mov	si,KRNL_SIZE
	mov	bx,ax	
	mov	ax,0x1000	
	mov	es,ax	
	mov	cx,2		
	xor	dx,dx	

.readAgain:
	push ax
	push bx
	mov	ax,0x0E00+'.'  
	mov	bx,7
	int	0x10	
	pop bx
	pop ax



	mov	ah,0x02	
	mov        al, 01	
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
	xor	ax,ax
	int	0x16
	int	0x19

.readOk:
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

	lgdt	[gdtinfo] 

	mov	eax,cr0  
	or	al,1    
	mov	cr0,eax  
	jmp dword (flat_code-gdt_table):pmode1

pmode1:
[BITS 32]
	
	mov	ax, flat_data-gdt_table	
	;mov	ax,0x10
	mov	ds, ax			
	mov	es, ax			
	mov	fs, ax	
	mov 	gs, ax
	mov	ss,ax
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

times 510-($-$$) db 0
dw 0xAA55



;----------------------------------------------------------------

