/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: physicalmem.h		|
	| Date: 00/00/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _MEMORY_PHYSICALMEM_H_
#define _MEMORY_PHYSICALMEM_H_

#include <kdef.h>

namespace memory{

	class PhysicalMem{
	public:
		static void Init();
		static void *PageFrameAlloc();
		static void *PageFrameFree(void *address);
		static dword getPageNumber();
		static dword getFreePageNumber();
	private:
		static dword pageframebitmap[32768];
		static dword pagenumber;
		static dword freepagenumber;
	};
}

#endif
