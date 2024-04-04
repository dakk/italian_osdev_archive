;;
;; OSSO - A Micro Kernel OS
;; Copyright (c) 2000 Alessandro Iurlano.
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
;; 02111-1307, USA.
;;


;%define DEBUG


BITS 16

org 0

global _start_kernel

_start_kernel:
;************ STARTING KERNEL!************

;	pusha
;	mov ax,13h
;	int 10h		; for special cases
;	popa

;	mov si,test3
;	call stampa

	mov ax,0x300
	mov ds,ax
	mov es,ax

	jmp 0x300:word (cont)

cont: 	push es
	mov ax,0
	mov fs,ax

	mov ah,8
	mov dl,080h
	int 013h

	mov si,600h
	mov [fs:si],ax
	mov [fs:si+2],bx
	mov [fs:si+4],cx
	mov [fs:si+6],dx
	mov [fs:si+8],di
	mov dx,es
	mov [fs:si+10],dx

	pop es
	xor eax,eax		; computing gdt physical address
	mov ax,cs
	xor ebx,ebx
	mov bx,gdt
	shl eax,4
	add eax,ebx
	mov [gdt_addr],eax	; storing result

	add eax,fine_gdt-gdt
	mov bx,idt
	mov [bx],ax
	add bx,6
	shr eax,16
	mov [bx],ax

	xor eax,eax		; computing idt physical address
	mov ax,cs
	xor ebx,ebx
	mov bx,idt
	shl eax,4
	add eax,ebx
	mov [idt_addr],eax	; storing result

	cli			; Disabling interrupts
	        		; they will be reactivated by kernel
	lgdt [gdt_reg]		; loads gdt

	lidt [idt_reg]		; loads idt

	mov eax,cr0		; enter protected mode
	or eax,1
	mov cr0,eax

.5:	in      al, 0x64	; Enabling A20
	test    al, 2
	jnz     .5
	mov     al, 0xD1
	out     0x64, al
.6:	in      al, 0x64
	test    al, 2
	jnz     .6
	mov     al, 0xDF
	out     0x60, al


	;; 8 is the first descriptor in table (CODE32)
	;; ready for the BIG jump to Prova_PM
	;; 	Dest EQU 0x80000+prova_PM
	jmp 8:dword (0x3000+prova_PM)


fermati:
	mov si,Stop
	call stampa

infinito: jmp infinito

Error:	jmp Error


;; returns next cluster in ax
;; starting cluster is in ax
Enter_next: db 'Next_start',13,10,0
End_next: db 'Next_end',13,10,0

stampac:
	pusha
	mov ah,0xE
	xor bx,bx
	mov cx,1
	int 0x10
	popa
	ret

stampa:
	pusha
stampa_loop:
	mov al,[si]
	cmp al,0
	je esci
	call stampac
	inc si
	jmp stampa_loop
esci:
	popa
	ret

stampav:
	pusha
	mov bx,ax
	shr ax,12
	call hexconv
	call stampac
	mov ax,bx
	shr ax,8
	and al,0xF
	call hexconv
	call stampac
	mov al,bl
	shr al,4
	call hexconv
	call stampac
	mov al,bl
	and al,0xF
	call hexconv
	call stampac
	mov al,32
	call stampac
	popa
	ret

hexconv:
	cmp al,10
	jb aass4
	add al,7
aass4:
	add al,48
	ret



BITS 32

prova_PM:

			; WE ARE IN PROTECTED MODE !!!!!!!!!!
        mov edx,0x3f2		; Stopping floppy
	mov al,0x0c
	out dx,al

	xor ax,ax
	mov ax,16		; second descriptor in table (DATA RW)
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov ss,ax
	mov esp,0x9FFFF

	xor ax,ax		; no ldt for now
	lldt ax

	;; Reprogramming IRQs
        mov     al,0x11	; 	 initialization sequence
	out     0x20,al	; 	 send it to 8259A-1
        dw   0x00eb,0x00eb	; 	 jmp $+2, jmp $+2
	out     0xA0,al	;	 and to 8259A-2
	dw   0x00eb,0x00eb
	mov     al,0x20	; 	 start of hardware int's (0x20)
	out     0x21,al
	dw   0x00eb,0x00eb
	mov     al,0x28	; 	 start of hardware int's 2 (0x28)
	out     0xA1,al
	dw   0x00eb,0x00eb
	mov	al,0x04	; 	 8259-1 is master
	out	0x21,al
	dw	0x00eb,0x00eb
	mov	al,0x02	; 	 8259-2 is slave
	out	0xA1,al
	dw	0x00eb,0x00eb
	mov	al,0x01	; 	 8086 mode for both
	out	0x21,al
	dw	0x00eb,0x00eb
	out	0xA1,al
	dw	0x00eb,0x00eb

	;; mask off all interrupts for now
	mov	al,0xFF	; PIC 1, IRQ 0 - 7
	out	0x21,al	; 
	mov	al,0xFF	; PIC 2, IRQ 8 - 15
	dw	0x00eb,0x00eb
	out	0xA1,al

	cld
stop:					; finished for now...
	;; Big jump to the kernel

	call 8:0x20000

	;; Stopping the computer before making any damage ...
	;; this should not be reached
	cli
	hlt

	;; start GDT data
gdt_reg:	dw fine_gdt-gdt	; table size
gdt_addr:	dd 0		; Physical address
gdt:
	;; Dummy Selector
	dd 0
	dd 0

	;; First Selector CODE 32 bit 4GB
	dw 0xFFFF		; LIMIT 0..15
	dw 0			; BASE 0..15
	db 0			; BASE 16..23
	db 10011010b		; FLAGS Present Exec Conforming APP (CODE)
	db 11001111b		; FLAGS + LIMIT 16..19 Gran. 4k 32bit
	db 0			; BASE 24..35

	;; Second Selector DATI 32 bit R/W 4GB
	dw 0xFFFF		; LIMIT 0..15
	dw 0			; BASE 0..15
	db 0			; BASE 16..23
	db 10010010b		; FLAGS Present Data APP (DATA R/W)
	db 11001111b		; FLAGS + LIMIT 16..19 Gran. 4k 32bit
	db 0			; BASE 24..35

fine_gdt:
	;; fille : times (50000) db 0


	;; start IDT data

idt_reg:	dw idt_fine-idt ; idt size
idt_addr:	dd 0		; idt physical address
idt:
	dd 0
	dd 0			; empty interrupt vector
idt_fine:

; Bootloader variables
BPB: times (62) db 0
BootDrive:	db 0 ; 62
; Cylinders:	dw 0 ; 63
Sectors:	dw 0 ; 65
Heads:		dw 0 ; 67
DataStart:	dw 0 ; 69
RootStart:	dw 0 ; 71
shift:	dw 3	; 73		; FAT shit value
festone:dw 0xff8 ; 75


c:	dw 0 ; variabiline per read_sector
s:	db 0
h:	db 0


Benvenuto:	db "****** Pre-Kernel Loader ******",13,10,0
Errore:		db "Error",13,10,0
Stop:		db "Pre-Kernel endless loop !",13,10,0
processormsg:   db 'Checking for 386+ processor: ',0
need386:        db 'Sorry: 8-8 Requires 386 or higher!',13,10,0
found386:       db 'OK! 8-8 is Loading...',13,10,0
loadkernel	db '  Loading Kernel ...',13,10,0
loadmodules	db '  Loading Modules ...',13,10,0
Modulitrovati1  db '  Found ',0
Modulitrovati2  db ' modules',13,10,0
Modulino	db '    Found a module...',0
Modulino2	db 'Read!',13,10,0
noncached	db 'Cache MISS',13,10,0

KERNEL		db 'KERNEL     '
DIR             db 'BOOT       '
offset		dw 4; modules offset
ultimadim	dw 0
nmoduli		dw 0
FAT: 		dw 0xFFFF ;"Cache" of FAT

test1:		db 'TEST 1',13,10,0
test2:		db 'TEST 2',13,10,0
test3:		db 'TEST 3',13,10,0


%ifdef DEBUG
	dbgpre:		db '[',0
	dbgpost:	db '] ',0
%endif


PMPM:		db "Protected Mode Active. Jump to the Kernel...",13,10,0

BITS 16
