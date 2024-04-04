/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: idt.h			|
	| Date: 28/12/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_CPU_IDT_H_
#define _HARDWARE_CPU_IDT_H_

#include <kdef.h>

#define IDT_SIZE 256
namespace hardware{

	namespace cpu{
		
		class IDT{
		public:
			static void Init();
			static bool Add(void (*func)(), size_t index);
			
			static bool Del(size_t index);
			
			static void setIDTR(qword *base, size_t num_desc);
		private:
			static qword idt[IDT_SIZE];
		};
	}
}
#endif
