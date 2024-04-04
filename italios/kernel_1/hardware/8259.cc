#include <io.h>
#include <bitops.h>
#include <8259.h>

namespace {
	word irq_mask;
}

//Inizializza l'8259
void init8259()
{
	outportb_p(M_PIC, ICW1);	// Inizio l'inizializzazione 
	outportb_p(S_PIC, ICW1);

	outportb_p(M_PIC + 1, ICW2_M);	//la base nel vettore di interrupt
	outportb_p(S_PIC + 1, ICW2_S);

	outportb_p(M_PIC + 1, ICW3_M);	//la cascata
	outportb_p(S_PIC + 1, ICW3_S);

	outportb_p(M_PIC + 1, ICW4);	//finisco l'inizializzazione
	outportb_p(S_PIC + 1, ICW4);

	irq_mask = 0xFFFF;
	outportb_p(M_PIC + 1, irq_mask & 0xFF);
	outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
}



/*abilita la ricezioni di un irq*/
bool enable_irq(word irq_no)
{
	//controllo se è un irq valido
	if (irq_no <= 15) {
		//faccio in modo che il bit corrispondente all'irq valga 0.
		//Es. se irq_no vale 6 succede ciò:
		//(1 << irq_no)   =  1000000
		//~(1 << irq_no) = 0111111
		//irq_mask &= ~(1 << irq_no);  =  xxxx xxxx x0xx xxxx
		irq_mask &= ~(1 << irq_no);

		//se l'irq è maggiore di 8 mi assicuro che la linea 2 (quella della cascata) sia attivata
		if (irq_no >= 8)
			irq_mask &= ~(1 << 2);

		//sendo all'8259 la nuova configurazione dell'IMR
		outportb_p(M_PIC + 1, irq_mask & 0xFF);
		outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
		return true;
	}
	return false;
}

/* disabilita la ricezione di un irq */
bool disable_irq(word irq_no)
{
	if (irq_no <= 15) {
		//mi occupo di mettere ad 1 il bit corrisponde all'irq che ci interessa
		//es. se irq_no vale 3
		//(1 << irq_no)  =  1000
		//irq_mask |= (1 << irq_no)  =  xxxx xxxx xxxx 1xxx
		irq_mask |= (1 << irq_no);

		//se gli irq alti (del pic slave) lo disabilito così è un pò + efficente ^__^
		if ((irq_mask & 0xFF00) == 0xFF00)
			irq_mask |= (1 << 2);

		outportb_p(M_PIC + 1, irq_mask & 0xFF);
		outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
		return true;
	}
	return false;
}


size_t get_cur_irq()
{
	byte irq;

	//indico che voglio leggere l'ISR       
	outportb_p(M_PIC, 0x0B);
	irq = inportb_p(M_PIC);

	//se l'8259 master non ha nessun interrupt attivo controllo lo slave
	if (irq == 0) {
		outportb_p(S_PIC, 0x0B);
		irq = inportb_p(S_PIC);
		return find_first_bit(&irq, sizeof(byte));
	}
	return find_first_bit(&irq, sizeof(byte));
}
