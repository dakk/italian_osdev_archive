#define DIM_IDT 256 //Numero di descrittori all' intero della IDT


//Questa è la struttura di un descrittore all' interno della IDT - Interrupt Descriptore Table

       struct descrittore
{
		
	        unsigned short int offset0_15;
		unsigned short int segmento;
		unsigned short int riservato : 5;
		unsigned short int opzioni : 11;
		unsigned short int offset16_31;
};



//Prototipi delle funzioni utilizzate


void initIDT(void);
void Init_IDT_entry(void (*handler)(void), int indice);
void Init_PIC8259(void);
void abilita_irq(unsigned char irq_num, unsigned char port_num);
char inportb  (int numport);
void outportb (int portdata, int portnum);
void handler_asm();
void gestisci_irq();
void handler_default(void);

//Prototipi degli handler delle eccezioni
void trap0(void);
void trap1(void);
void trap2(void);
void trap3(void);
void trap4(void);
void trap5(void);
void trap6(void);
void trap7(void);
void trap8(void);
void trap9(void);
void trap10(void);
void trap11(void);
void trap12(void);
void trap13(void);
void trap14(void);
void trap15(void);
void trap16(void);
void trap17(void);
////Fine prototipi degli handler delle eccezioni


