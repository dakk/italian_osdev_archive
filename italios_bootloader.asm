;ItaLo italiOs Loader
;funziona solo con processori a 32 bit
; il kernel dal terzo settore ed è lungo KRNL_SIZE settori
;si va poi in modalità protetta e si fa un jump al codice del kernel
; che è un normale binario

KRNL_SIZE	equ	 50	;al momento una grandezza di 50 è + che sufficente

SECTION .text

	org	0x7C00  ;deve essere caricato in ram a partire da questo indirizzo
	
main:
	xor	ax,ax	;azzero ax
	mov	ds,ax	;setto il selettore dei dati a 0
	mov	ss,ax	;idem per il selettore dello stack
	mov	sp,0xFFFE	;setto lo stack poienter ad 0xFFFE così il kernel ha ababstanza spazio


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


	mov	si,KRNL_SIZE	;lo usiamo come una specie di contatore
	mov	bx,ax	;metto ax a zero
	mov	ax,0x1000	;metto in ax l'offset in cui caricare il kernel per poi spostarlo in es
	mov	es,ax	;quindi setto es con il mio offset
	mov	cx,2		;metto cx a 2 per indicargli di partire dal secondo settore
	xor	dx,dx	;azzero di dx

.readAgain:
	push ax
	push bx
	mov	ax,0x0E00+'.'  
	mov	bx,7
	int	0x10	;stampo il carattere '='
	pop bx
	pop ax



	mov	ah,0x02		;indico che voglio leggere
	mov        al, 01	;leggo solo un settore
	int	0x13	;leggo dal floppy
	jc	short .error		;se c'è un errore	prova la lettura 3 volte (altrimenti il floppy è marcio)

	mov	ax,es	;
	add	ax,32		;	incrementento es di 32 byte
	mov	es,ax	;

	dec	si	;decremento il contatore
	jz	short .readOk	;se si è diverso da 0 leggo il settore successivo

	inc	cx		;incremento il contatore dei settori
	cmp	cl,18	;controllo se è finita la traccia
	jbe	short .readAgain	;se non è ancora finita leggo il setrtore successivo
	mov	cl,1	;altrimenti metto la traccia ad 1
	inc	dh	;e metto la seconda faccia
	cmp	dh,2		
	jne	short .readAgain	;se non sono alla seconda faccia leggo ancora
	mov	dh,0	;altrimenti torno alla prima faccia
	inc	ch	;e incremento ancora la testina
	jmp	short .readAgain	;e vado a leggere


.error:	;se ci è stato un errore in lettura
	xor	ax,ax	;metto ax a zero
	int	0x16	;aspetto la pressione di un tasto
	int	0x19	;riavvio il caricamente del bootloader

.readOk:

	; spengo il motore del floppy
        mov edx,0x3f2	;metto edx=0x3f2 che è il comando per spegnere il floppy
        mov al,0x0c	;metto al=0xC che è la porta del floppy
        out dx,al	;eseguo il comando
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
	mov	esp,  0x9FFFC	;imposto lo stack

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

