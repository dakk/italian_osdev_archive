/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: paging.h			|
	| Date: 23/09/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _MEMORY_PAGING_H_
#define _MEMORY_PAGING_H_

#include <kdef.h>

namespace memory{

	enum PagingBits{
		Present = 1,
		Write = 2,
		User = 4,
		WriteThrough = 8,
		PageLevelCacheDisable = 16,
		Accessed = 32,
		Dirty = 64,
		PageSize = 128,
		PAT = 128,
		Global = 256
	};

	#define KERNEL_STD_PAGE Present | Write
	#define KERNEL_NOPRESENT_STD_PAGE Write

	class Paging{
	public:
		static void Init();

		static void Enable();
		static void Disable();
		static dword NewPTE(void *address, dword flags);
		static dword NewPDE(dword *pagetable, dword flags);
		static void setCR3(dword cr3reg);
		static void PageFaultISR();
		static void PhyscalAlloc(dword addr, dword size);
	private:

	};

}

#endif
