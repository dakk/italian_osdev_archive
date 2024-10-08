/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: gdt.h			|
	| Date: 10/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_CPU_GDT_H_
#define _HARDWARE_CPU_GDT_H_

#include <kdef.h>

#define GDT_SIZE 8192 - 1 //Per qualche motivo non funziona se non metto -1 TODO: Controllare il codice

namespace hardware{

	namespace cpu{
	
		enum GDTSegmentType{
			//Dati
			stDataReadOnly = 0,
			stDataReadOnlyAccessed = 1,
			stDataReadWrite = 2,
			stDataReadWriteAccessed = 3,
			stDataReadOnlyExpandDown = 4,
			stDataReadOnlyExpandDownAccessed = 5,
			stDataReadWriteExpandDown = 6,
			stDataReadWriteExpandDownAccessed = 7,
			
			//Codice
			stCodeExecuteOnly = 8,
			stCodeExecuteOnlyAccessed = 9,
			stCodeExecuteRead = 10,
			stCodeExecuteReadAccessed = 11,
			stCodeExecuteOnlyConforming = 12,
			stCodeExecuteOnlyConformingAccessed = 13,
			stCodeExecuteReadOnlyConforming = 14,
			stCodeExecuteReadOnlyConformingAccessed = 15,
		};
		
		class GDT{
		public:
			static void Init();
			
			static void AddDescriptor();
			
			static int GDT::AddMemorySegment(int limit, int base, int ring,bool granularity, GDTSegmentType type, bool bit32, bool SegmentPresent);
			
			static bool Del(int index);
			
			static void setGDTR(qword * base, size_t num_desc);
			
			static int getFirstFreeDescriptor();
		private:
			static qword gdt[GDT_SIZE];
		};
	}
}

#endif
