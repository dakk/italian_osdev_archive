;===============================================================================
;= ItaLo italiOs Loader ========================================================
;===============================================================================
;=                                                                             =
;= Funziona solo con processori a 32 bit il kernel dal terzo settore ed è      =
;= lungo KRNL_SIZE settori si va poi in modalità protetta e si fa un jump al   =
;= codice del kernel che è un normale binario.                                 =
;=                                                                             =
;===============================================================================
;= Autori ======================================================================
;===============================================================================
;= Silvio "Kernel2420" Abruzzo ( silvioabruzzo@italios.it )                    =
;= "Razziatore" ( IlRazziatore@netscape.net )                                  =
;===============================================================================

%include "kernel_len.inc"


SECTION .text

  org 0x7C00  ;deve essere caricato in ram a partire da questo indirizzo

main:
  xor ax,ax      ; azzero ax
  mov ds,ax      ; setto il selettore dei dati a 0
  mov ss,ax      ; idem per il selettore dello stack
  mov sp,0xFFFE  ; setto lo stack poienter ad 0xFFFE
                 ; così il kernel ha ababstanza spazio

  jmp start      ; saltiamo le procedure e inziamo sul serio

;===============================================================================
;= RestoreCursor ===============================================================
;===============================================================================
;=                                                                             =
;= Sposta il cursore alla posizione salvata con SaveCursor.                    =
;= Se non è ancora stata salva il cursore vera posizionato all'origine         =
;=                                                                             =
;===============================================================================

RestoreCursor:
  ;
  ; INT 10 02-- - VIDEO - SET CURSOR POSITION
  ;
  ; Inp.:
  ;   AH = 02h
  ;   BH = page number
  ;        0-3 in modes 2&3
  ;        0-7 in modes 0&1
  ;        0 in graphics modes
  ;   DH = row (00h is top)
  ;   DL = column (00h is left)
  ;

  push ax
  push bx
  push dx

  mov ah, 0x02
  xor bx, bx
  mov dx, [ CURSOR_POSITION ]

  int 0x10

  pop dx
  pop bx
  pop ax

  ret

;===============================================================================
;= SaveCursor ==================================================================
;===============================================================================
;=                                                                             =
;= Sposta il cursore alla posizione salvata con SaveCursor.                    =
;= Se non è ancora stata salva il cursore vera posizionato all'origine         =
;=                                                                             =
;===============================================================================

SaveCursor:
;
; INT 10 03-- - VIDEO - GET CURSOR POSITION AND SIZE
;
; Inp.:
;   AH = 03h
;   BH = page number
;        0-3 in modes 2&3
;        0-7 in modes 0&1
;        0 in graphics modes
;
; Return: AX = 0000h (Phoenix BIOS)
;   CH = start scan line
;   CL = end scan line
;   DH = row (00h is top)
;   DL = column (00h is left)
;
; Notes:
;   A separate cursor is maintained for each of up to 8 display pages
;   many ROM BIOSes incorrectly return the default size for a color display
;   (start 06h, end 07h) when a monochrome display is attached
;

  push ax
  push bx
  push cx
  push dx

  mov ax, 0x03
  xor bx, bx

  int 0x10

; mov [ SCAN_LINE ], cx
  mov [ CURSOR_POSITION ], dx

  pop dx
  pop cx
  pop bx
  pop ax

  ret

;===============================================================================
;= PutChar =====================================================================
;===============================================================================
;= display a character on the screen, advancing the cursor and scrolling the   =
;= screen as necessary                                                         =
;=                                                                             =
;= Input:                                                                      =
;=   AL = character to write                                                   =
;=                                                                             =
;= Notes:                                                                      =
;=   characters 07h (BEL), 08h (BS), 0Ah (LF), and 0Dh (CR) are interpreted    =
;=   and do the expected things IBM PC ROMs dated 4/24/81 and 10/19/81         =
;=   require that BH be the same as the current active page                    =
;=                                                                             =
;===============================================================================

PutChar:
  ;
  ; INT 10 0E-- - VIDEO - TELETYPE OUTPUT
  ;
  ; Inp.:
  ;   AH = 0Eh
  ;   AL = character to write
  ;   BH = page number
  ;   BL = foreground color (graphics modes only)
  ;
  ; Desc:
  ;   display a character on the screen, advancing the cursor and scrolling
  ;   the screen as necessary
  ;
  ; Notes:
  ;   characters 07h (BEL), 08h (BS), 0Ah (LF), and 0Dh (CR) are interpreted
  ;   and do the expected things IBM PC ROMs dated 4/24/81 and 10/19/81
  ;   require that BH be the same as the current active page
  ;

  push ax
  push bx

  mov ah, 0x0E
; mov al, char
  xor bx, bx

  int 0x10

  pop bx
  pop ax

  ret

;===============================================================================
;= WriteString =================================================================
;===============================================================================
;=                                                                             =
;= Scrive una stringa terminate con 0                                          =
;=                                                                             =
;= Input:                                                                      =
;=   SI = Address of string                                                    =
;=                                                                             =
;===============================================================================

  ; prints message in reg. si
WriteString:
  pusha
_n:
  lodsb
  or    al,al
  jz    short _e
  mov   ah,0x0E
  mov   bx,7
  int   0x10
  inc word [CURSOR_POSITION]
  jmp   _n
_e:
  popa
  ret

;===============================================================================
;= CalcPerc ====================================================================
;===============================================================================
;= Calcola la percentuale di un valore rispetto alla base                      =
;=                                                                             =
;= Input:                                                                      =
;=   AX = Valore                                                               =
;=   BX = Base                                                                 =
;=                                                                             =
;= Return:                                                                     =
;=   AX = Percentuale                                                          =
;=                                                                             =
;===============================================================================

CalcPerc:

  push dx
  push cx

  ;valore * 100 = pippo              pippo/base = risultato

  ;mov ax, bx
  mov cx, 0x64 ; 100d

  mul cx

  div bx

  pop cx
  pop dx

  ret

;===============================================================================
;= PutNumber ===================================================================
;===============================================================================
;=                                                                             =
;= Scrive un numero                                                            =
;=                                                                             =
;= Input:                                                                      =
;=   AL = Valore                                                               =
;=                                                                             =
;===============================================================================

A db 0
B db 0
C db 0

PutNumber:

  push ax
  push bx
  push dx

; mov al, Numero
  xor ah, ah
  mov bx, 100

  cwd
  idiv bx

  mov [A], al
  mov al, dl
  xor ah, ah
  mov bx, 10

  cwd
  idiv  bx

  mov [B], al
  mov [C], dl

  cmp byte [A], 0

  je PutNumber_86

  mov al, [ A ]
  add al, 0x30

  call PutChar

PutNumber_86:

  cmp byte [B],0
  jne PutNumber_142

  cmp byte [A],0
  je  PutNumber_170

PutNumber_142:

  mov al, [B]
  add al, 0x30

  call PutChar

PutNumber_170:

  mov al, [C]
  add al, 0x30

  call PutChar

  pop dx
  pop bx
  pop ax

  ret

;===============================================================================
;==================================== START ====================================
;===============================================================================

start:
  call SaveCursor

  mov si, LOADING_STRING

  call WriteString

  mov si,KRNL_SIZE ;metto in si la dimenzione del ccs+kernel
  mov bx,ax ;metto ax a zero
  mov ax,0x1000 ;metto in ax l'offset in cui caricare il kernello
  mov es,ax ;il buffer per i dati è es:bx
  mov cx,2
  xor dx,dx

.readAgain:
  push ax
  push bx

  mov ax, KRNL_SIZE

  sub ax, si

  mov bx, KRNL_SIZE

  call RestoreCursor

  call CalcPerc

  call PutNumber

  mov al, '%'

  call PutChar

  pop bx
  pop ax

;INT 13 - DISK - READ SECTOR(S) INTO MEMORY
; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
;      high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer
;       Return: CF set on error
;      if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some
;       BIOSes)

  mov ax,0x0201
  int 0x13  ;leggo dal floppy
  jc  short .error    ;se c'è un errore prova la lettura 3 volte (altrimenti il floppy è marcio)

  mov ax,es
  add ax,32
  mov es,ax

  dec si
  jz  short .readOk

  inc cx
  cmp cl,18
  jbe short .readAgain
  mov cl,1
  inc dh
  cmp dh,2
  jne short .readAgain
  mov dh,0
  inc ch
  jmp short .readAgain


.error:
;error handler
  mov ax,0x0E00+'e'   ; se ci sono problemi di letturastampo una e a video e aspetto la pressione di un tasto
  mov bx,7
  int 0x10
  xor ax,ax
  int 0x16
  int 0x19

.readOk:

  push si

  mov si, DONE_STRING

  call WriteString

  pop Si

  ; spengo il motore del floppy
        mov edx,0x3f2
        mov al,0x0c
        out dx,al

  ;setto l'80x25
  ;mov ax, 1112h
  ;int 0x10

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

  mov eax, 0x10000

  call eax

  cli
  hlt


;----------------------- GDT -------------------------------------

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

;START BOOTSECTOR HEADER
BYTES_PER_SECTOR equ  512
SECTORS_PER_TRACK equ 18
TOTAL_HEADS equ 2
TOTAL_SECTORS equ 2880   ; 1474560/512 for a 1.44meg disk


;END BOOTSECTOR HEADER

bootdrv          db  0

CURSOR_POSITION  dw  0x0000

LOADING_STRING   db  'ItaLo 0.2 Loading boot image...', 0

DONE_STRING      db  'done', 0

times 510-($-$$) db 0
dw 0xAA55



;----------------------------------------------------------------

