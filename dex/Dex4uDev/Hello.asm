;=========================================================;
; Test calltable                                 06/12/05 ;
;---------------------------------------------------------;
; By Dex4u.                                               ;
;                                                         ;
; Here is a simple "hello world" program.                 ;
; for Dex4u, using the calltable functions.               ;
; To assemble use fasm as follows                         ;
; c:\fasm hello.asm hello.dex                             ;
;=========================================================;
use32
        ORG   0x200000          ; where our program is loaded to
        jmp   start             ; jump to the start of program.
        db    'DEX1'            ; We check for this, to make shore it a valid Dex4u file.

msg1:   db 'Hello world! ',13,0  

 ;----------------------------------------------------;
 ; Start of program.                                  ;
 ;----------------------------------------------------;

start:
        mov   ax,18h            ; set ax to nonlinear base
        mov   ds,ax             ; add them to ds
        mov   es,ax             ; and es.

 ;----------------------------------------------------;
 ; Get calltable address.                             ;
 ;----------------------------------------------------;

        mov   edi,Functions     ; this is the interrupt
        mov   al,0              ; we use to load the DexFunction.inc
        mov   ah,0x0a           ; with the address to dex4u functions.
        int   40h 

 ;----------------------------------------------------;
 ; Try print string.                                  ;
 ;----------------------------------------------------;

        mov   esi,msg1          ; this point's to our string.
        call  [PrintString]     ; this call the print function.
        call  [WaitForKeyPress] ; is the wait for keypress function.
        ret                     ; This returns to the CLI/GUI

 ;----------------------------------------------------;
 ; Data.                                              ;
 ;----------------------------------------------------;

include 'DexFunctions.inc'      ; Here is where we includ our "DexFunctions.inc" file