;#####################
;#SIAMO IN REAL MODE #
;#####################


DIM_KERNEL	equ	 42

[BITS 16]       ;è codice a 16 bit
[ORG 0x7C00]    ;L'indirizzo di partenza. l' indirizzo 0x7c00 infatti indica proprio l' indirizzo del bootsector ovvero il primo settore che il bios carica


xor ax, ax
mov ds, ax
mov ss,ax
mov sp,0xFFFE 
mov bx, WELCOME

call write   ;Chiamiamo la routine che ci scrive a video il messaggio il cui indirizzo è contenuto nel registro 'bx'
call newline ;Chiamiamo la routine che porta il cursore su una nuova linea. Svolge la stessa funzione del /n in C


mov bx, KERNELOAD
call write



call load_kernel ;il kernel viene caricato nei vari registri ma non viene lanciato. Dovrà essere lanciato in modalità
		 ;prottetta e non in real mode dove adesso ci troviamo
call off_floppy



;#############################################
;# CI PREPRARIAMO PER ANDARE IN PROTECT MODE #
;#############################################

init_pmode:

;1 - disabilitamo gli interrupt
	call newline
	mov bx, DISINT
	call	write
	call	newline
	cli

	in	al,0x70		;disabilito i NMI
	or	al,0x80
	out	0x70,al

;2 - abilitiamo la linea a20

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

;3 - carichiamo la gdt
	mov bx, LOADGDT
	call	write
	call	newline

	lgdt	[gdtinfo]

;4 - settiamo il bit PE
	mov bx, BITPE
	call	write
	call	newline
	mov	eax,CR0
	or	al,1
	mov	CR0,eax

;5 - saltiamo al codice a 32 bit
	jmp	dword (flat_code-gdt_table):lancia_kernel




;####################################################
;# SIAMO IN PROTECT MODE, QUINDI LANCIAMO IL KERNEL #
;####################################################



lancia_kernel:
[BITS 32]       ;codice a 32 bit

        mov  ax,flat_data-gdt_table	;metto 0x10 in ax
        mov ds, ax    	;imposto ds
        mov es, ax    	;imposto es
        mov fs, ax     	;imposto fs
        mov   gs, ax  	;imposto gs
	mov   ss, ax  	;imposto ss

  mov esp,  0x9FFF0     ;imposto lo stack

        push dword 2  ;azzero gli EFLAG
        popfd   ;e li imposto

        mov eax, 0x10000        ;la locazione di memoria dove c'è il kernel

        jmp eax        ;lancio il kernel!!!

	cli
	hlt


[BITS 16]
load_kernel:

mov di, DIM_KERNEL ;Nel registro 'di' va messo la dimensione in settori del kernel
mov ax, 0x1000  ;copio 0x1000 in ax
		mov es, ax      ; e quindi imposto il segmento

		mov ch, 0       ;imposto il numero della traccia
		mov cl, 2       ;imposto il numero del settore

		mov dh, 0       ;numero della testina
		mov dl, 0       ;numero del drive

		xor bx, bx      ;metto a zero il buffer         es:bx  = 0x1000:0

	leggi:
                mov ah, 2       ;imposto il numero del servizio
		mov al, 1       ;indico quanti settori leggere

		int 0x13	;ora leggo davvero
		jc fdc_error	;se c'è stato un errore salto alla gestione dell'errore

	        push bx         ;salvo il valore di bx
		mov bx, READ_MESS	;metto in bx l'indirizzo del messaggio
		call write   	;e quindi lo stampo
		pop bx          ;ripristiamo il valore di bx

		mov ax,es       ;metto il valore di es in ax
		add ax,32       ;quindi modifico questo valore
		mov es,ax       ;e riaggiorno es

		dec di          ;decrementiamo di uno il numero dei settori
		jz _fine ;abbiamo finito


		inc cx  ;incremento il numero del settore
		cmp cl,18	;comparo cl e 18
		jbe leggi       ;se è < 18 allora posso continuare a leggere
		mov cl,1        ;altrimenti riinizio dal settore 1
		inc dh  ;incremento il numero della testina
		cmp dh,2	;comparo dh e 2
		jne leggi       ;se è la testina 1 allora tutto bene
		mov dh,0       ;altrimenti imposto la testina 0
		inc ch  ;e incremento il numero della traccia
		jmp leggi	;ora tutto è apposto posso continuare a leggere

	fdc_error:	;gestione dell'errore
		mov bx, FDC_ERROR	;metto in bx l'indirizzo del messaggio di errore
		call write  	; e lo stampo
	_fine:	;fine della routine
	ret




;Stampa una stringa in formato C, cioè torminante con uno '\0'
;INPUT:
;bx = indirizzo del primo elemnto della stringa
;OUTPUT: nessuno
write:
                pusha   ;salvo tutti i registri
		push es ;salvo lo stato di es
		xor ax, ax      ;metto a 0 ax
		mov es, ax	;metto es a 0
	        xor di, di      ;metto a 0 di, che rappresenterà un contatore di caratteri
        conteggio:      ;ciclo che conta i caratteri e man mano li stampa
	        cmp  byte[es:bx+di], 0  ;controllo se il carattere corrente p 0
	        je fine         ;se è così significa che la stringa è finita
		mov ah, 0Eh     ;metto in ah 0xEh che rappresenta il codice del servizio per stampare
		mov al, byte[es:bx+di]  ;metto in al il carattere corrente

		push bx ;salvo il valore di bx
		mov bh, 0       ;imposto la pagina video
		int 10h         ;stampo il carattere
		pop bx  ;ripristino bx

		inc di  ;incremento il contatore
		jmp conteggio   ;salto per stampare il carattere successivo
	fine:   ;gestione della fine della stampa
	        pop es          ;ripristino il valore originale di es
        	popa            ;ripristino il valore dei registri
	        ret             ;abbiamo finito




;Piccola funzione che stampa a video uno '\n'
newline:
        push bx         ;salvo il valore di bx
        mov bx, NEWLINE_MESS    ; metto in bx l'indirizzo dello \n
	call write     ;lo stampo
	pop bx          ;ripristino il valore di bx
	ret                    ;esco dalla funzione







;============================================================
;GDT

gdtinfo:        ;la strttura da caricare con lgdt

dw  gdtlength   ;il numero di elementi presenti nella gdt
dd  gdt_table   ;l'indirizzo della gdt


	
gdt_table:

null_desc:
    dd  0	;il primo elemento con valore nullo
    dd  0	;double dword  a 0

flat_code:
    dw  0xFFFF  ; limite bit   0->15
    dw  0x0000  ; Base bit 0->15
    db  0x00  ; base bit  16->23
    db  0x9A  ; segmento di codice presente con privilegio 0
    db  0xCF  ; limite bit 16->19 segmento a 32 bit step: 4096 byte
    db  0x00  ;  base 24->32

flat_data:
    dw  0xFFFF  ; limite bit   0->15
    dw  0x0000  ; Base bit 0->15
    db  0x00  ; base bit  16->23
    db  0x92  ; segmento di dati presente con privilegio 0
    db  0xCF  ; limite bit 16->19 segmento a 32 bit step: 4096 byte
    db  0x00  ;  base 24->32

 gdtlength equ $ - gdt_table  ;imposto il valore della lunghezza 

;====================================================================


off_floppy:
  mov dx,0x3f2
  mov al,0x0c
  out dx,al
  ret

;################################
;# MESSAGGI DA STAMPARE A VIDEO #
;################################

WELCOME      db  '-Bewoot-',0
KERNELOAD      db  'Caricamento del kernel',0
FDC_ERROR db 'Errore nella lettura del floppy', 0
READ_MESS db '.', 0
ACTA20      db  'Abilitazione linea di indirizzamento a20',0
DISINT      db  'Disabilitazione degli interrupt',0
LOADGDT    db  'Caricamento della GDT',0
BITPE    db  'Bit PE settato',0
NEWLINE_MESS db 13,10,0

times 510-($-$$) db 0           ;riempiamo tutto lo spazio fino a 510byte di 0
;times	((18*2*80*512)-($-boot)) db 0
dw 0xAA55       ;il nostro carattere terminatore







