/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: irq.h			|
	| Date: 30/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_CPU_IRQ_H_
#define _HARDWARE_CPU_IRQ_H_

#include <kdef.h>

#define M_PIC	0x20		// I/O for master PIC              
#define S_PIC	0xA0		// I/O for slave PIC               

#define M_VEC	0x30		//Dove viene mappato IRQ0
#define S_VEC	M_VEC+8		//Dove viene mappato IRQ8

#define ICW1	0x11		// 0001   0001      
	
#define ICW2_M	M_VEC		// ICW2 per il master
#define ICW2_S	S_VEC		// ICW2 per lo slave
	
#define ICW3_M	(1 << 2)	// ICW3 per il master
#define ICW3_S	3		// ICW3 per lo slave

#define ICW4	0x01		//ICW4

#define IRQ_NUM 16

namespace hardware{

	namespace cpu{
		
		class IRQ{
		public:
			static void Init();
			static bool EnableIRQ(int irq_no);
			static bool DisableIRQ(int irq_no);
			
			static bool Add(void (*func) (), byte number);
			static bool Del(byte number);
		private:
			static word irq_mask;
			static void Init8259();
			static size_t getCurrentIRQ();
		};
	}
}

#endif
