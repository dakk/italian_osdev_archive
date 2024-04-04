%define ROOT_SEG	0x60
%define FAT_SEG		0x800
%define IMAGE_SEG	0x1000

%if ROOT_SEG & 31
  %error "ROOT_SEG DEVE essere divisibile per 0x20"
%endif
%if ROOT_SEG > 0xC00
  %error "Root directory deve stare sotto i  64Kb"
%endif
%if FAT_SEG & 31
  %error "FAT_SEG DEVE essere divisibile per 0x20"
%endif
%if FAT_SEG > 0xE80
  %error "FAT deve stare sotto i 64Kb"
%endif
%if IMAGE_SEG & 31
  %error "IMAGE_SEG DEVE essere divisibile per 0x20"
%endif

	org	0x7C00

entry:
	jmp	short begin
	nop

%include "fat_header.inc"

begin:
	xor	ax, ax
	mov	ds, ax
	mov	ss, ax
	mov	sp, 0xFFFE 
	mov	bp, sp
	mov	[BS_DrvNum], dl	;numero del driver

	mov	al, [BPB_NumFATs]	;numero di FAT
	mul	word [BPB_FATSz16]	; * Settori per FAT
	add	ax, [BPB_RsvdSecCnt]	; + Settori prima della FAT
				;AX = Settori della Root

	mov	si, [BPB_RootEntCnt]	;Massimo numero di entry delle directory
	mov	cl, 4
	dec	si
	shr	si, cl
	inc	si		;SI = lunghezza dei settori della root

	mov	di, ROOT_SEG/32	;buffer
	call	read_16		;leggo la root
	push	ax		;settore del secondo cluster
%define sc_clu2 bp-2		;memorizzp per + tardi l'indirizzo del settore

	mov	dx, [BPB_RootEntCnt]	;numero di directory
	push	ds
	pop	es
	mov	di, ROOT_SEG*16

search:
	dec	dx		;altre direcotry
	js	error2		;No
	mov	si, filename	;vedo se è il nome che sto cercando
	mov	cx, 11		;lunghezza 11 caratteri
	lea	ax, [di+0x20]	;memorizzo il successivo indirizzo
	push	ax
	repe cmpsb		;Comparo
	pop	di
	jnz	search		;ripeto finche ci sono dir

	push word [di-6]	;numero del cluster di inizio

	mov	ax, [BPB_RsvdSecCnt]	;numero del settore della fat
	mov	si, [BPB_FATSz16]	;lunghezza della fat
	mov	di, FAT_SEG/32	;buffer
	call	read_16		;leggo la fat

next:
	pop	bx		;numero del cluster
	mov	si, bx		;primo cluster
	mov	ax, bx		;ultimo cluster

.0:
	cmp	bx, 0xFF8	;sono alla fine del file?
	jae	.2		; si
	inc	ax		;ultimo cluster + 1

	;guardo nella FAT il cluster successivo
	mov	di, bx		;numero del cluster
	rcr	bx, 1		;1.5 byte èer ogni cluster
				;bx = 0x8000 + cluster/2

	mov	bx, [bx+di+FAT_SEG*16-0x8000]
	jnc	.1
	shr	bx, 1
	shr	bx, 1
	shr	bx, 1
	shr	bx, 1
.1:	and	bh, 0xF

	cmp	ax, bx		;il successivo claster è continguo?
	je	.0		;si
.2:	sub	ax, si		;quanti ce ne sono in sequenza?
	jz	eof		;basta il file è finito

	push	bx		;salvo il successivo cluster
	
	mov	bl, [BPB_SecPerClus]	;settori per cluster
	mov	bh, 0		;  come word
	mul	bx		;lunghezza dei settori in sequenza
.3:	mov	di, IMAGE_SEG/32 ;destinazione
	add	[.3+1], ax	 ;salvo la destinazione successiva
	xchg	ax, si		;AX = inizio del cluster ;SI = lunghezza in settori
	dec	ax
	dec	ax		;inizio dei cluster meno 2
	mul	bx		; * settori per cluster
	add	ax, [sc_clu2]	; + numero del settore del secondo cluster
	adc	dl, dh		;memorizzo il risultato

	call	read_32		;leggo
	jmp	short next	;guardo dopo


error2:	
	mov	si, 	notfound	
	mov	ax, 0xE0D	
	mov	bx, 7
.1:	int	10h
	lodsb
	test	al, al
	jnz	.1
	xor	ah, ah
	int	16h		;aspetto un tasto
	int	19h		;riavvio


eof:

  cli
.wait1:
  in  al, 0x64
  test  al, 2
  jnz .wait1
  mov al, 0xD1
  out 0x64, al

.wait2:
  in  al, 0x64
  and ax, byte 2
  jnz .wait2
  mov al, 0xDF
  out 0x60, al

  lgdt  [gdtinfo]  ;carico la gdt

  mov eax,cr0   ;  metto in eat cr0
  or  al,1      ;imposto il bit per la modalità protetta
  mov cr0,eax   ;siiiiiiiii, vado in modalità protetta
  jmp dword (flat_code-gdt_table):pmode1; tutto è pronto, e quindi eseguo il kernel

pmode1:
[BITS 32]


  mov ax, flat_data-gdt_table ; Selector for 4Gb data seg
  ;mov  ax,0x10
  mov ds, ax      ; {2} Extend limit for ds
  mov es, ax      ; Extend limit for es
  mov fs, ax      ; fs and...
  mov   gs, ax      ; gs
  mov ss,ax
  mov esp,  0x9FFF0

  push dword 2
  popfd

	cli
  mov eax, 0x10000

  call eax

  cli
  hlt
[bits 16]
error:	mov	si, errmsg	
	mov	ax, 0xE0D	
	mov	bx, 7
.1:	int	10h
	lodsb
	test	al, al
	jnz	.1
;	xor	ah, ah
;	int	16h		
	int	19h		

read_16:
	xor	dx, dx

read_32:
;
; Input:
;    dx:ax = settori
;    si    = contatore dei settori
;    di    = segmento di destinazione / 32
;
; Il numero del settore è convertito nella tripletta testina settore cilindro
; e i settori letti vengono scritti in (di*32):0
;
; Output:
;    dx:ax  aggiornato
;    di     aggiornato
;    si = 0
;    bp, ds preservato
;    bx, cx, es modificato

.1:	push	dx			;settore relativo alto
	push	ax			;settore relativo basso

	add	ax, [BPB_HiddSec]		
	adc	dx, [BPB_HiddSec+2]

	mov	bx, [BPB_SecPerTrk]	
	div	bx		
	sub	bx, dx	
	cmp	bx, si		
	jbe	.2		
	mov	bx, si	
.2:	inc	dx	
	mov	cx, dx	
	cwd				
	div	word [BPB_NumHeads]	
	mov	dh, dl		

	xchg	ch, al		
	ror	ah, 1			
	ror	ah, 1
	add	cl, ah			

	sub	ax, di
	and	ax, byte 0x7F	
	jz	.3
	cmp	ax, bx		
	jbe	.4		
.3:	xchg	ax, bx
.4:	push	ax		
	mov	bx, di		
	push	cx
	mov	cl, 5
	shl	bx, cl
	pop	cx
	mov	es, bx
	xor	bx, bx			
	mov	dl, [BS_DrvNum]	
	mov	ah, 2			
	int	13h		
	jc	error
	pop	bx			
	pop	ax			
	pop	dx			
	add	ax, bx		
	adc	dl, dh
	add	di, bx		
	sub	si, bx		
	jnz	.1			
	ret

errmsg	db	"FM",0

notfound	db "KERNEL NON TROVATO",0


gdtinfo:

dw  gdtlength
dd  gdt_table


gdt_table:

null_desc dd  0
    dd  0

flat_code dw  0xFFFF  ;
    dw  0x0000  ; Base == 0x00000000
    db  0x00  ; Limit == 0xFFFFFFFF
    db  0x9A  ; Flag1 == 0x4 == b0100 (G, D/B, reserved, AVL)
    db  0xCF  ; Flag2 == 0x9A == b10011010 (P, DPL, S, Type)
    db  0x00  ;

flat_data dw  0xFFFF  ;
    dw  0x0000  ; Base == 0x00000000
    db  0x00  ; Limit == 0xFFFFF
    db  0x92  ; Flag1 == 0x4 == b0100 (G, D/B, reserved, AVL)
    db  0xCF  ; Flag2 == 0x92 == b10010010 (P, DPL, S, Type)
    db  0x00  ;

gdtlength equ $ - gdt_table


size	equ	$ - entry


%if size+11+2 > 512
 %error "Codice troppo grande per un bootloader"
%endif
times	(512 - size - 11 - 2) db 0

filename db	"KERNEL  BIN"		;nome
	db	0x55, 0xAA
