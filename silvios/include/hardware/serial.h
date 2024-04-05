#ifndef _SERIAL_H_
#define _SERIAL_H_


#define NO_DEVICE 123456  //numero preso a caso
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


//riceve un byte dalla seriale number, se non c'è niente viene restituito NO_DATA in polling
unsigned int uart_getchar_polling(unsigned short int number);

//riceve una stringha dalla seriale number in polling
void uart_gets_polling(unsigned short int number, unsigned char *string);

//butta una stringha fuori dalla seriale in polling
void uart_puts_polling(unsigned short int number, unsigned char *string);

//butta un carattere fuori dalla seriale in polling
void uart_putchar_polling(unsigned short int number, unsigned char car);

//prende un byte dalla seriale ma in interrupt. Se non ci sono dati viene restituito NO_DATA
unsigned int uart_getchar_interrupt(unsigned short int number);

//prende una stringha dalla seriale ma in interrupt
void uart_gets_interrupt(unsigned short int number, unsigned char *string);

/*apre e inizializza una seriale
  number: indica il numero della seriale
  mode: può valere INTR (modalità in interrupt) o POLLING. Se si usa la modalità in
   	interrupt possono anche essere usate le funzioni per il polling
  velocità: indica la velocità in kb/s
  parita: indica come se la parità deve essere pari o dispari 
  len_word: indica quanto deve essere lungha la parola
  stop_bit: indica come deve essere fatto il i di stop
*/
unsigned int uart_open(unsigned char number, unsigned char mode,
		       unsigned int velocita, unsigned char parita, unsigned char len_word, 
		       unsigned char stop_bit);
		       
		       

//cerca le seriali prenti nel pc e le inizializza
void detect_UARTs();

//setta la velocità della porta numero number
unsigned int set_UART_speed(unsigned short int number, unsigned int speed);

#endif
