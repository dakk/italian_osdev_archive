[BITS 16]
[ORG 0x7C00]

xor ax, ax
mov ds, ax
mov ss, ax
mov sp, 0xFFFE

mov bp, WELCOM
mov cx, 26
call Stampastringa

mov bp, WELCOM1
mov dh, 1
mov dl, 0
mov cx, 22
call Stampastringa

mov bp, LEGGIKERNEL
mov dh, 2
mov dl, 0
mov cx, 18
call Stampastringa
call Posiziona
call carica_kernel
call spegni_floppy

mov bp, KERNELDONE
mov dh, 3
mov dl, 0
mov cx, 22
call Stampastringa

caricapmode:
	mov bp, PMODELOAD
	mov dh, 4
	mov dl, 0
	mov cx, 17
	call Stampastringa
	cli

waitfree1:
	in al, 0x64
	test al, 2
	jnz waitfree1
		mov al, 0xD1
		out 0x64, al
waitfree2:
	in al, 0x64	
	and ax, byte 2
	jnz waitfree2
	mov al, 0xDF
	out 0x60, al

lgdt[info_gdt]

mov eax,cr0   ;
or  al,1      ;
mov cr0,eax   ;

jmp dword (gdt_code-gdt_table):pmode

[BITS 32]
pmode:

	mov  ax,gdt_data-gdt_table
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov   gs, ax
	mov   ss, ax

	mov esp,  0x9FFFC
	
        push dword 2
        popfd

       mov eax, 0x10000

       jmp eax
       
       cli
       hlt

[BITS 16]
Stampastringa:
	mov ah, 13h
	mov al, 0
	xor bx, bx
	mov bh, 0
	mov bl, 7h
	int 10h
	ret

Posiziona:
	mov ah, 02h
	mov bh, 0
	mov dh, 4
	mov dl, 0
	int 10h
	ret

StampaUguale:
	mov ah, 0eh
	mov al, '='
	mov bh, 0
	mov bl, 0
	push dx
	mov dh, 4
	mov dl, 0
	int 10h
	pop dx
	mov ah, 2
	mov al, 1
	ret

spegni_floppy:
  mov dx,0x3f2
  mov al,0x0c
  out dx,al
  ret

carica_kernel:
		mov di, 50
		mov ax, 0x1000
		mov es, ax

		mov ch, 0
		mov cl, 2

		mov dh, 0
		mov dl, 0
		
		xor bx, bx

	leggi:
                mov ah, 2
		mov al, 1
	
		int 0x13
		call StampaUguale
		jc fdc_error

	        push bx
		pop bx

		mov ax,es 
		add ax,32
		mov es,ax

		dec di
		jz _fine

		
		inc cx
		cmp cl,18
		jbe leggi
		mov cl,1
		inc dh
		cmp dh,2
		jne leggi
		mov dh,0
		inc ch
		jmp leggi

	fdc_error:
		mov bx, KERNELERRO
		call Stampastringa
	_fine:
	        ret

info_gdt:
	dw lunghezza
	dw gdt_table

gdt_table:

	gdt_null_desc:
		dd 0
		dd 0
	gdt_code:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x9A
		db 0xCF
		db 0x00
	
	gdt_data:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x92
		db 0xCF
		db 0x00
	
lunghezza equ $ - gdt_table

WELCOM	db  'Benvenuto in uos '
WELCOM1 db  'bootloader per uos'
LEGGIKERNEL db 'Caricamento Kernel'
KERNELDONE db 'Caricamento Completato'
KERNELERRO db 'Errore Lettura'
PMODELOAD db 'Caricamento pmode'

times 510-($-$$) db 0
dw 0xAA55
