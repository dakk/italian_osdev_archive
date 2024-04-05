/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: inthandler.h		|
	| Date: 29/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_CPU_INTHANDLER_H_
#define _HARDWARE_CPU_INTHANDLER_H_

#include <kdef.h>

namespace hardware{

	namespace cpu{
		
		class IntHandler{
		public:
			static void IntNull();
		
			static void Int0(); //#DE - Divide Error
			static void Int1(); //#DB - Debug
			static void Int2(); //NMI Interrupt
			static void Int3(); //#BP - Breakpoint
			static void Int4(); //#OF - Overflow
			static void Int5(); //#BR - BOUND Range Exceeded
			static void Int6(); //#UD - Invalid Opcode
			static void Int7(); //#NM - Device Not Avaible
			static void Int8(); //#DF - Double Fault
			//static void Int9();
			static void Int10(); //#TS - Invalid TSS
			static void Int11(); //#NP - Segment Not Present
			static void Int12(); //#SS - Stack-Segment Fault
			static void Int13(); //#GP - General Protection
			static void Int14(); //#PF - Page Fault
			//static void Int15();
			static void Int16(); //#MF - Floating-Point Error
			static void Int17(); //#AC - Alignment Check
			static void Int18(); //#MC - Machine Check
			static void Int19(); //#XF - Streaming SIMD Extensions
		};
	}
}

#endif
