
 /*ItaliOs
 
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

#include <irq.h>
#include <8259.h>
#include <idt.h>



using namespace itos;
using namespace itos::IA32;

extern "C"{

	void (*handler[16])();

	extern void internal_handler(void);

	void null_handler(){ return;}

	void free_irq(byte number){
		if(number < 8)
			outportb(m_pic, eoi);
		else
			outportb(s_pic, eoi);
		return;
	}

	void do_irq(unsigned int *esp, byte number){
		(*handler[number])();
//		scheduler(esp);
		return;
	}
	
	void  _do_irq(unsigned int esp){
		byte current_irq = get_cur_irq();
		do_irq(&esp, current_irq);
		free_irq(current_irq);
		return;
	}
	

	asm(".globl internal_handler        \n"
		"internal_handler:              \n"
		"cli	\n"
		"pusha	\n"
		"movl %esp, %eax	\n"
		"pushl %eax	\n"
		"                       \n"
		"call _do_irq   \n"		
		"popl %eax	\n"
		"mov %eax, %esp	\n"
		"popa	\n"
		"sti	\n"
		"iret                \n"	/* Exit interrupt                   */
		);


	void add_irq(void (*func)(), byte number){
		handler[number]=func;
		return;
	}
}




void init_irq(){
		Init8259();
		for(int i = 0; i < 16; i++){
			if(i < 8)
				idt::add(internal_handler, m_vec+i);
			else
				idt::add(internal_handler, s_vec+i);
			add_irq(null_handler, i);
		}
		return;
}


