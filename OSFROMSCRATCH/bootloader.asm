mov     si,KRNL_SIZE    ;lo usiamo come una specie di contatore
        mov     bx,ax   ;metto ax a zero
        mov     ax,0x1000       ;metto in ax l'offset in cui caricare il kernel per poi spostarlo in es
        mov     es,ax   ;quindi setto es con il mio offset
        mov     cx,2            ;metto cx a 2 per indicare di partire dal secondo settore
        xor     dx,dx

.readAgain:
        mov     ah,0x02         ;indico che voglio leggere
        mov        al, 01       ;leggo solo un settore
        int     0x13    ;leggo dal floppy
        jc      short .error    ;se c'è un errore prova la lettura 3 volte (altrimenti il floppy è difettoso)

        mov     ax,es   ;
        add     ax,32           ;       incrementento es di 32 byte
        mov     es,ax   ;

        dec     si      ;decremento il contatore
        jz      short .readOk   ;se si è diverso da 0 leggo il settore successivo

        inc     cx              ;incremento il contatore dei settori
        cmp     cl,18   ;controllo se è finita la traccia
        jbe     short .readAgain        ;se non è ancora finita leggo il settore successivo
        mov     cl,1    ;altrimenti metto la traccia ad 1
        inc     dh      ;e metto la seconda faccia
        cmp     dh,2
        jne     short .readAgain        ;se non sono alla seconda faccia leggo ancora
        mov     dh,0    ;altrimenti torno alla prima faccia
        inc     ch      ;e incremento ancora la testina
        jmp     short .readAgain        ;e vado a leggere


.error: ;se ci è stato un errore in lettura
        xor     ax,ax   ;metto ax a zero
        int     0x16    ;aspetto la pressione di un tasto
        int     0x19    ;riavvio il caricamente del bootloader

.readOk:

        ; spengo il motore del floppy
        mov edx,0x3f2   ;metto edx=0x3f2 che è il comando per spegnere il floppy
        mov al,0x0c     ;metto al=0xC che è la porta del floppy
        out dx,al       ;eseguo il comando
	cli     ;disabililito gli interrupt
.wait1:                 ;aspetto che la tastiera sia libera
        in      al, 0x64
        test    al, 2
        jnz     .wait1
        mov     al, 0xD1
        out     0x64, al

.wait2:         ;aspetto che la tastiera sia disposta ad accettare comandi
        in      al, 0x64
        and     ax, byte 2
        jnz     .wait2
        mov     al, 0xDF
        out     0x60, al
	lgdt    [gdtinfo]  ;carico la gdt

        mov     eax,cr0   ;  metto in eat cr0
        or      al,1      ;imposto il bit per la modalità protetta
        mov     cr0,eax   ;siiiiiiiii, vado in modalità protetta
        jmp dword (flat_code-gdt_table):pmode1;configuro i selettori dei segmenti

pmode1:
[BITS 32] 
 mov     ax, flat_data-gdt_table ; metto il selettore in ax
        mov     ds, ax                  ; uso lo stesso selettore per i dati
        mov     es, ax                  ; l'extra segment
        mov     fs, ax                  ; fs
        mov     gs, ax          ; gs
        mov     ss,ax                   ; e lo stack
        mov     esp,  0x9FFFC   ;imposto la base dello stack

        push dword 2            ;metto il valore 2 nello stack
        popfd                           ;imposto a 2 gli eflag
 	mov eax, 0x10000
        call eax

