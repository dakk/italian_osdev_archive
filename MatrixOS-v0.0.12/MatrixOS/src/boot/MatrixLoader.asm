
;	+---------------------------------------+
;	|		MatrixOS		|
;	+---------------------------------------+
;	| Filename: MatrixLoader.asm		|
;	| Date: 00/00/2004			|
;	| Authors: Michele			|
;	| License: GNU General Public License	|
;	+---------------------------------------+

[BITS 16]
[ORG 7C00h]			;carichiamo in memoria questo file da 7C00

	jmp INIZIO		;saltiamo al codice da eseguire

settori_logici equ 250	;indichiamo quanti settori dobbiamo caricare in totale!!!
;*************************************************************************
;						GDT
gdtr:
	dw	gdt_size	;Base Table Limit (dimesione della tabella GDT)
	dd	gdt_table	;Linear Base Address (posizione in memoria)

gdt_table:
	times 8 db 0		;i primi 8 byte devono essere sempre a zero (manuale Intel)
gdt_code:				;descrittore del codice
	dw	0FFFFh		;segment limit 0-15
	dw	0000		;base address 0-15
	db	00			;base address 16-23
	db	09Ah		;'1 00 1 1010' (P DPL S Type)
	db	0CFh		;'1 1 0 0 1111' (G D/B 0 AVL Seg.Limit19:16)
	db	00			;base address 24-31
gdt_data:				;descrittore dei dati
	dw	0FFFFh		;segment limit 0-15
	dw	0000		;base address 0-15
	db	00			;base address 16-23
	db	92h			;'1 00 1 0010' (P DPL S Type)
	db	0CFh			;'1 1 0 0 1111' (G D/B 0 AVL Seg.Limit19:16)
	db	00			;base 24-31

gdt_size equ $-gdt_table	;dimenzione della GDT

;*************************************************************************
;						Routine varie
svuota_8042:
        call delay		;aspettiamo un attimo
	in al,64h       	;prendiamo il dato
	test al,1		;se (bit al[0]==0) 
	jz no_output	;non ci sono dati (buffer vuoto), quindi...
	call delay		;altrimenti aspettiamo un attimo
	in al,60h		;prendiamo il dato
	jmp svuota_8042;e ripetiamo 
no_output:
	test al,2		;se (bit al[1]==1)
	jnz svuota_8042;andiamo a risvuotare il bus
	ret
delay:
	dw 0EBh	;ritardo
	ret
;*************************************************************************
;						Inizio del codice
INIZIO:
	cli
	xor ax, ax			;azzeriamo i registri bs, ds e es e impostiamo la stack a FFFE
	mov bp, ax	
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0FFFEh	;Imposto lo Stack
	sti

;************************************
; inizio routine di lettura da floppy
	mov ax,1000h		;Carichiamo il kernel da (ES:BX = 1000:0000)
	mov es,ax			;ES 1000
	mov bx,0h			;BX 0000
	mov al,01h		;settori da leggere
	mov ch,00h		;cilindro 0
	mov cl,02h		;si inizia a leggere dal 2 perchè nell'uno c'è il bootstrap
	mov dh,00h		;testina
	mov dl,00h		;floppy num.0
	mov di,settori_logici	;settori logici
leggi_floppy:
	mov ah,02h		;servizio
	int 13h			;GO!!!
	dec di			;decrementiamo il contatore perchè abbiamo letto il settore!
	push ax			;salviamo ax
	mov ax,es
	add ax,32			;andiamo a scrivere nei prossimi 32 byte della memoria
	mov es,ax
	pop ax			;riprendiamo ax
	cmp di,0			;abbiamo letto tutti i settori logici?
	je esci_leggifloppy	;se si, smettiamo di leggere
	inc cl			;altrimenti incrementiamo il settore da leggere
	cmp cl,19			;siamo arrivati al 18° settore?
	je cambiatraccia	;allora cambiamo la traccia
	jmp leggi_floppy	;altrimenti rileggiamo
cambiatraccia:
	mov cl,1			;rileggiamo dal 1° settore
	inc dh			;cambiamo la testina
	cmp dh,2			;stiamo leggendo la 2° testina?
	jne leggi_floppy		;se no, continuamo a leggere il floppy
	mov dh,0			;altrimenti leggiamo la 1° testina 
	inc ch			;incrementiamo la traccia
	jmp leggi_floppy	;e rileggiamo

esci_leggifloppy:		;abbiamo finito di leggere, quindi...
	mov edx,3F2h		;...spengo il floppy (ma questo comando spenge solo il driveA o tutti i drive?)
	mov al,0Ch
	out dx,al
; fine routine di lettura da floppy
;************************************

	cli				;disabilito i CLI (interrupt mascherabili)
;	in al,70H			;disabilito i NMI (interrupt non mascherabili)
;	or al,80H			;così sia con CLI che con NMI è tutto disattivato!
;	out 70H,al			;ovviamente per gli NMI dobbiamo agire sul chip CMOS !

        call svuota_8042
	mov al,0D1H
	out 64H,al			; command: write to the output port
        call svuota_8042
	mov al,0DFH
	out 60H,al			;se il bit 1 èsettato = A20 abilitato
        call svuota_8042
	lgdt [gdtr]			;carichiamo la GDT nel registro GTDR

	mov eax,CR0		;settiamo PE. Infatti non basta abilitare l'A20
	or al,1			;ma dobbiamo abilitire questo bit per entrare in pmode
	mov CR0,eax
		
jmp dword (gdt_code-gdt_table):pmode	; tutto e' pronto, e quindi eseguo il codice a 32 bit

	
; ******************* BIT 32 ****************************
[BITS 32]		
pmode:
	mov  ax,gdt_data-gdt_table;metto 0x10 in ax
	mov ds,ax    		;imposto ds
	mov es,ax    		;imposto es
	mov fs,ax     		;imposto fs
	mov gs,ax  		;imposto gs
	mov ss,ax  		;imposto ss

	mov esp,  0x9FFFC	;imposto lo stack
	push dword 2		;azzero gli EFLAG
	popfd   			;e li imposto
	mov eax, 0x10000	;la locazione di memoria dove ci sta kernel
	jmp eax			;GO !!! (kernel)
	cli				;disabilitiamo gli interrupts
	hlt				;ferma tutto!!!

	times 510-($-$$) db 0
	dw 0xAA55	; numero da mettere in fondo al file per far riconoscere al BIOS che il settore è un bootstrap
