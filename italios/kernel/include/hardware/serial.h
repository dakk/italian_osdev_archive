#ifndef _SERIAL_H_
#define _SERIAL_H_


#define NO_DEVICE NULL
#define NO_DATA 654321  //numero preso a caso

#define INTR 1
#define POLLING 0

#define TX  0x00
#define RX  0x00
#define RDR  0x00
#define DLL 0x00
#define DLM 0x01
#define IER 0x01
#define IIR 0x02
#define FCR 0x02
#define LCR 0x03
#define MCR 0x04
#define LSR 0x05
#define MSR 0x06
#define SR  0x07


//costanti per la parità
#define	NO_PARITY	0
#define	ODD_PARITY  8
#define	EVEN_PARITY  24
#define	HIGH_PARITY  40
#define	LOW_PARITY  56

//costanti per la lunghezza di parola
#define	BITS_5_PER_WORD	0
#define	BITS_6_PER_WORD	1
#define	BITS_7_PER_WORD	2
#define	BITS_8_PER_WORD	3


//costanti per il bit di stop
#define	STOP_BIT_1	0
#define	STOP_BIT_2	4       //2 bit di stop per parole lunghe 6/7/8 bit e 1.5 bit di stop


#define TRASMISSIONE 0x20
#define RICEZIONE 0x01

#define MAXSPEED 115200

//definizione delle varie maschere di settaggio e rilevazione degl'interrupts

#define IDMASK 0x06 //da applicare a IIR

#define EINT_MODEM 0x08 //mascherature da applicare su IER per abilitare
#define EINT_LINEA 0x04 //il relativo interrupt
#define EINT_TX    0x02
#define EINT_RX    0x01

#define INT_LINEA  0x06 //da testare su IIR dopo averlo mascherato
#define INT_RX     0x04
#define INT_TX     0x02
#define INT_MODEM  0x00


#define   UARTNOTPR  0    // non presente
#define   UART8250       1    // 8250.
#define   UART16450   2        // 16450 or 8250 with scratch reg.
#define   UART16550_NOSCR 3 // 16550 with no scratch register.
#define   UART16550 4        // 16550.
#define   UART16550A_NOSCR 5 // 16550A with no scratch register.
#define   UART16550A       6 // 16550A.

#define DEFAULT_IRQ 70   //valore preso a caso, ma con cognizione di causa preso sopra 15

#define DIM_BUFFER  0x1000


struct bufferpos {
	unsigned int fine;	//numero di byte dall'inizio del blocco
	unsigned char *inizio;	//puntatore all'inizio del blocco di memoria
	unsigned int posw;	//indica quanti bytes sono stti scritti
	unsigned int posr;	//indica quanti bytes sono stti lettti
};

/*descrittore della seriale*/
struct serial_info {
	unsigned short int address;  /*indirizzo in cui è memorizzata la seriale*/
//	unsigned char chip;  /*tipo di chip*/
	unsigned char irq;  /*numero dell'irq*/
	unsigned char *buffer; //puntatore al buffer che sarà grande DIM_BUFFER se si apre la porta in polling la variabile punterà a NULL
	struct bufferpos pos;
};

typedef struct serial_info SERIALE;

//riceve un byte dalla seriale
unsigned int uart_getchar_polling(SERIALE* ser);

//riceve una stringha dalla seriale
void uart_gets_polling(SERIALE* ser, unsigned char *string);

//butta una stringha nella seriale
void uart_puts_polling(SERIALE* ser, unsigned char *string);

//butta un carattere nella seriale
void uart_putchar_polling(SERIALE* ser, unsigned char car);


/*apre una seriale, l'ultimo parametreo indica l'irq esso può essere settato in DEFAULT_IRQ per far usare il valore di default, oppure si può specificare manualmente l'irq da usare
il parametreo mode può assumere i valori INTR e POLLING
se ci sono problemi la funzione restituerà NULL
*/
SERIALE* uart_open(unsigned char number, unsigned char mode,
		       unsigned int velocita, unsigned char parita,
		       unsigned char len_word, unsigned char stop_bit, unsigned char irq);

//rilevazione della seriale	       
void init_uart();

//setta la velocità della seriale
unsigned int set_UART_speed(SERIALE * ser, unsigned int speed);

//aggiunge un carattere al buffer della seriale
void add_char_to_buffer(SERIALE * ser, unsigned char car);

//prene un carattere dal buffer della seriale
unsigned int uart_getchar_interrupt(SERIALE *ser);

//prende una stringha
void uart_gets_interrupt(SERIALE *ser, unsigned char *string);

#endif
