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


; ************************************************************
;                   Configuration defines.
; ************************************************************

;; *** Select in MUTUAL EXCLUSION!!!
;	%define AUTOFAT  ; enable automatic FAT recognition
;	%define FAT12    ; enable explicit FAT12 code
;	%define FAT16    ; enable explicit FAT16 code

;; *** Enable check for 'FAT1' string.
;	%define FATCHECK

;; *** Use generic bytes per sector code (instead of BpS=512).
;	%define GENERIC_BxS

;; *** Enable printing of '!' when an error occurs.
	%define PRINTERR


;; *** Enable code for bcc generated pre-kernel
	%define BCCLOADER

;; *** Uses complete root scanning.
	%define SCANROOTFIX

;; *** Enable printing of '#' when a read error occurs.
	%define PRINTREADERR

;; *** Enable retry to read defective sectors. (See below).
	%define READRETRY

;; *** Specifies how many times times we must retry to read.
	%define NUM_READ 5

;; *** Disables 32 bit code in sector calculation (without
;;     this bootloader can't work on hard disks).
;;     This must be used only for debug purpose!!!
;	%define DISABLE_32_BIT

; ************************************************************


%define HIDDEN_SECTORS       dword[0x1c]
%define HIDDEN_SECTORS_L     word[0x1c]
%define HIDDEN_SECTORS_H     word[0x1e]

%define BYTES_PER_SECTOR     word[0x0b]
%define SECTORS_PER_CLUSTER  byte[0x0d]
%define RESERVED_SECTORS     word[0x0e]
%define NUMBER_OF_FATS       byte[0x10]
%define NUM_ROOT_ENTRIES     word[0x11]
%define SECTORS_PER_FAT      word[0x16]


%macro GET_NEXT_CLUSTER 0

        ;; Returns in ax next cluster number.
        ;; Starting cluster must be in ax.

                push bx
        ;       push dx
        ;       push cx
                push es

                xor dx,dx
                mul word[shift]
                mov cx,ax
                shr ax,1
                and cx,1

        %ifdef GENERIC_BxS
                mov bx,BYTES_PER_SECTOR
        %else
                mov bx,512
        %endif

                div bx                  ; ax: fat sector, dx: offset
                mov es,bx

                cmp ax,[FAT]            ; check if fat sector is already
                je letturaprossimo      ; in memory
                mov [FAT],ax
                add ax,RESERVED_SECTORS

                xor bx,bx
                call locate_a

                add bx,BYTES_PER_SECTOR
                inc ax
                call locate_a

        letturaprossimo:
                mov bx,dx
                mov ax,[es:bx]

                cmp word[shift],4
                je festa
                cmp cl,0
                je anda
                shr ax,4
        anda:
                and ax,0x0FFF

        festa:
                pop es
        ;       pop cx
        ;       pop dx
                pop bx
%endmacro


%macro CLUSTER_TO_SECTOR 0

        ;; Returns in ax the sector corresponding
        ;; to the cluster passed (in ax).

        ;       push dx
                xor cx,cx
                mov cl,SECTORS_PER_CLUSTER      ; Sectors per Cluster
                mul cx
                add ax,DataStart
        ;       pop dx
%endmacro





start:  jmp short inizio

dummy:  times (62-($-$$)) db 0

BootDrive:      db 0 ; 62
Sectors:        dw 0 ; 63
sect_w:         dw 0
Heads:          dw 0 ; 67
head_w:         dw 0

%define DataStart       word[512]
%define RootStart       word[514]


%ifdef FAT12 AUTOFAT
        shift:  dw 3            ; shift value for FAT
        festone:dw 0xff8
%endif
%ifdef FAT16
        shift:  dw 4            ; shift value for FAT
        festone:dw 0xfff8
%endif


inizio:  ; start
        cli
        mov ax,0x7c0            ; setting segments values
        mov ds,ax
        mov es,ax
        mov ax,0x9000
        mov ss,ax
        mov sp,0xFFF8
        sti

        ;; Returns disk geometry:
        ;; ah 0 OK!
        ;; bl Drive type
        ;; ch 8 low bits of max cylinder
        ;; cl 0..5 max sector / 6.7 high byte cylinder
        ;; dh max head
        ;; es:di point. parameters table.
        ;; heads and cyls go from 0 to max

        mov [BootDrive],dl
        mov ah,08
        push es
        int 0x13
        pop es
        jc Error

        xor dl,dl
        xchg dl,dh
        inc dx
        mov [Heads],dx
        mov dx,cx
        shr dl,6
        xchg dh,dl
        inc dx
;       mov [Cylinders],dx

        and cl,63
        mov [Sectors],cl
        ;; Cyl Sect Heads seem correct 80, 18, 2

        mov dl,[BootDrive]
        xor ah,ah
        int 0x13

        mov ax,SECTORS_PER_FAT
        xor bx,bx
        mov bl,NUMBER_OF_FATS
        mul bx
        add ax,RESERVED_SECTORS
        mov cx,ax
        mov ax,NUM_ROOT_ENTRIES

%ifdef GENERIC_BxS
        mov bx,BYTES_PER_SECTOR ; Bytes per Sector
        shr bx,5                ; Entries per Sector (BpS/32)
        div bx
%else
        shr ax,4
%endif

        add ax,cx
        mov DataStart,ax
        mov RootStart,cx

%ifdef FATCHECK
        mov si,FATTYPE
        mov cx,4
        mov di,0x36
        repz
        cmpsb
        jcxz vai
%else
        jmp short vai
%endif

Error:
%ifdef PRINTERR
        mov ax,0xE21
Error2:
        xor bx,bx
        mov cx,1
        int 0x10
%endif
iflop:  jmp short iflop

vai:

%ifdef AUTOFAT
        mov al,[0x3A]
        cmp al,'2'
        je fat12
        cmp al,'6'
        jne Error

fat16:
        inc byte[shift]
        mov byte[festone],0xff

fat12:
%endif


via:
        mov bx,0x2000
        mov es,bx
        mov ax,RootStart

rootget:
        xor bx,bx
        call locate_a

ricerca:
        mov cx,11
        mov si,LOADER
        mov di,bx
        repe
        cmpsb
        jcxz trova
        add bx,32

%ifdef SCANROOTFIX

        %ifdef GENERIC_BxS
                cmp bx,BYTES_PER_SECTOR
        %else
                cmp bx,512
        %endif

        jl ricerca

        inc ax
        cmp ax,DataStart
        jl rootget

        jmp short Error
%else
        jmp short ricerca
%endif

trova:
        ; The entry is right.
        ;
        add bx,0x1a
        mov ax,[es:bx]

        xor bx,bx
        mov ch,0x80             ; CAUTION: cl must be 0 here!!!
                                ; This should be 'cx,0x8000'
        mov es,cx
ancora:
        call locate

  GET_NEXT_CLUSTER

%ifdef GENERIC_BxS
        push ax
        xor ax,ax
        mov al,SECTORS_PER_CLUSTER
        mov cx,BYTES_PER_SECTOR
        mul cx                  ; ax = SectPerCluster * BytesPerSector
        add bx,ax
        pop ax
%else
        add bh,SECTORS_PER_CLUSTER      ; Double increment simulate
        add bh,SECTORS_PER_CLUSTER      ; add bx,512*SECTORS_PER_CLUSTER
%endif

        cmp ax,[festone]
        jnae ancora

partenza:

%ifdef BCCLOADER
        mov bx,0x8000
        push bx
        mov ds,bx
        mov es,bx
        inc bx
        inc bx
        xor di,di
        mov ax,[di]
        cmp ax,0x301
        jnz bad_magic
        mov ax,[di+2]
        and ax,0x20
        jz impure
        mov cl,4
        mov ax,[di+8]
        shr ax,cl
impure:
        add ax,bx
        mov ss,ax
        mov sp,[di+24]
        mov ds,ax
        push bx
bad_magic:
        push di
        retf
%else
        jmp 0x8000:0
%endif

locate_a:
        pusha
        mov cx,1
        jmp short pio

locate:                         ; ax = number of cluster
        pusha
        dec ax
        dec ax

        CLUSTER_TO_SECTOR

pio:
        push ax
%ifdef DISABLE_32_BIT
        %define BXREG bx
%else
	and eax,0xffff
        add eax,HIDDEN_SECTORS
        %define BXREG ebx
%endif

%ifdef READRETRY
        mov cx,NUM_READ
try:
        push cx
%endif
        push bx
        mov BXREG,[Sectors]
	xor dx,dx
        div BXREG
        inc dx
        xor cx,cx
        mov cl,dl               ; Set sector
        xor dx,dx
        mov BXREG,[Heads]
        div BXREG
        mov dh,dl               ; Set head
        mov dl,[BootDrive]      ; Set boot drive
        shl ah,6
        xchg ah,al
        or cx,ax                        ; Set cylinder
        mov ah,2                        ; Set type of service (read sector)
        mov al,SECTORS_PER_CLUSTER      ; Set number of sector to read
        pop bx                          ; Set offset of address
        int 0x13                        ; Read the sector

%ifdef READRETRY
        pop cx
%endif
        jnc ok
%ifdef READRETRY
        loop try
%endif
%ifdef PRINTREADERR
        mov ax,0xE23
        %ifdef PRINTERR
                jmp Error2
        %else
                xor bx,bx
                mov cx,1
                int 0x10
        iflop2: jmp short iflop2
        %endif
%else
        %ifdef PRINTERR
                jmp Error
        %else
        err1:   jmp short err1
        %endif
%endif

ok:
        pop ax
        inc ax
        popa
        ret



FAT:    dw 0xFFFF               ;"Cache" of FAT
FATTYPE:db "FAT1"
LOADER: db "LOADER     "
fille:  times (510-($-$$)) db 0
dw 0xAA55 ; boot signature
