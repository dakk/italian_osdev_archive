 /*
 ItaliOs
 
 Copyright  (c) <2003> Abruzzo Silvio
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
7
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <debug.h>
#include <idt.h>
#include <video.h>
#include <irq.h>
	
namespace {

        //come è strutturato un int_gate
        struct int_gate{
	        word offset_low;
		word segment;
		word reserved : 5;
		word option : 11;
		word offset_high;
	};

	//handler di default: non fa nulla
	void mint_null(){   
	       kout << "\nè arrivato qualcosa";
	 asm("iret");      
	 }
	
	//numero di interrupt che usiamo (il massimo!!!)
	static const size_t _num_elem = 256;
	
	//il nostro bellissimo vettore di interrupt ^__^
	int_gate  _IDT[_num_elem];	
	
	//funzione per impostare l'idtr
	void set_idtr(int_gate* base,size_t num_desc) {
		dword idt_reg[2];
		idt_reg[0]=(num_desc*8) << 16;
		idt_reg[1]=(dword)base;
		__asm__ __volatile__ ("lidt (%0)": :"g" ((char *)idt_reg+2));
	}
	
	void isr0(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "int0: errore divisione per 0!!!";
		while (1);
	}
		
	void isr1(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint1: eccezione di debug!!!\n";
		while (1);
	}

	void isr2(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint2: nmi!!!\n";
		while (1);
	}

	void isr3(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint3: breakpoint!!!\n";
		while (1);
	}

	void isr4(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint4: overflow!!!";
		while (1);
	}


	void isr5(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint5: bound!!!";
		while (1);
	}
	
	void isr6(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint6: invalid opcode!!!";
		while (1);
	}


	void isr7(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint7: no fpu!!!";
		while (1);
	}


	void isr8(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint8: double fault!!!";
		while (1);
	}


	void isr9(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint9: fpu overrun!!!";
		while (1);
	}


	void isr10(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint10: invalid tss!!!";
		while (1);
	}


	void isr11(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint11: segment not present!!!";
		while (1);
	}

	void isr12(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint12: errore nello stack!!!";
		while (1);
	}


	void isr13(){
		kout.set_text_color(ROSSO);
		kout << endl;
		kout << "\nint13: general protection !!!";
		while (1);
		}

		void isr14(){
			kout.set_text_color(ROSSO);
			kout << endl;
			kout << "\nint14: page fault!!!  (pagina non trovata)";
			while (1);
		}

		void isr16(){
			kout.set_text_color(ROSSO);
			kout << endl;
			kout << "\nint16: errore nella fpu";
			while (1);
		}


		void isr17(){
			kout.set_text_color(ROSSO);
			kout << endl;
			kout << "\n\nint17: memoria non allineata";
			while (1);
		}

}

namespace idt{

bool add(void (*func)(), size_t index){
        if(index < _num_elem){  //controlliamo se è un interrupt valido
	        debug << "Imposto l'handler " << index << endl;
	        dword addr = (dword)func;
                _IDT[index].offset_low = (addr & 0xFFFF);       //prendiamo i 16 bit + bassi
		_IDT[index].offset_high = (addr >> 16); //prendiamo i 16 bit + alti
		_IDT[index].segment = 0x8;      //questo è il segmento codice
		_IDT[index].option = 0x470;   //100  0111  0000 (PRESENTE  DPL=0)
		return true;
	}
	debug << "Impossibile imposta  l'handler " << index << " si trova fuori dal range. Massimo 256" << endl;
        return false;
}


bool add(void (*func)(int), size_t index){
        if(index < _num_elem){  //controlliamo se è un interrupt valido
	        debug << "Imposto l'handler " << index << endl;
	        dword addr = (dword)func;
                _IDT[index].offset_low = (addr & 0xFFFF);       //prendiamo i 16 bit + bassi
		_IDT[index].offset_high = (addr >> 16); //prendiamo i 16 bit + alti
		_IDT[index].segment = 0x8;      //questo è il segmento codice
		_IDT[index].option = 0x470;   //100  0111  0000 (PRESENTE  DPL=0)
		return true;
	}
	debug << "Impossibile imposta  l'handler " << index << " si trova fuori dal range. Massimo 256" << endl;
        return false;
}


void init(){
        debug << "Inizializazione degli interrupt in corso....." << endl;
	//impostiamo tutti gli interrupt con l'handler di default
        for(size_t i = 0; i < _num_elem; i++)
                add(mint_null, i);
		
	//setto le eccezzioni	
	add(isr0, 0);
        add(isr1, 1);
        add(isr2, 2);
        add(isr3, 3);
        add(isr4, 4);
        add(isr5, 5);
        add(isr6, 6);
        add(isr7, 7);
        add(isr8, 8);
        add(isr9, 9);
        add(isr10, 10) ;
        add(isr11,11);
        add(isr12, 12);
        add(isr13, 13) ;
        add(isr14, 14);
        add(isr16, 16);        
	add(isr17, 17);        
	
	init_irq();
	
	//settiamo l'idtr
	set_idtr(_IDT, _num_elem);
	
	//attiviamo gli interrupt
	asm("sti");
}

	//cancella l'handler di un interrupt e restituisce il puntatore al precedente handler
void* del(size_t index){
        if(index < _num_elem){
	        void *handler = get(index);
		//se è 0 è già installato mint_null, quindi evitiamo di reistallarlo in questo modo è molto + veloce
		if(!handler)
	                add(mint_null, index);
		return handler;
	}
	return (void *)0;
}

	//restituisce l'indirizzo di un handler
	void* get(size_t index){
	        if(index < _num_elem){
		        void *handler;
			handler =(void*) ((_IDT[index].offset_high<<16) + _IDT[index].offset_low);
			//se è l'handler di default cioè mint_null, restituiamo 0, altrimenti se poi uno fa eseguire mint_null avrebbe dei problemi, inoltre non ci sarebbe modo da parte dell'utente di capire se è installato l'handler di default
		if(handler ==((void *) mint_null))
		        handler = (void *)0;
		return handler;
		}
		return (void *)0;
	}

}
