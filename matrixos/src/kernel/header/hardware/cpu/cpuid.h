/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: irq.h			|
	| Date: 19/10/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_CPU_CPUID_H_
#define _HARDWARE_CPU_CPUID_H_

#include <kdef.h>

namespace hardware{

	namespace cpu{
		
		class CPUID{
		public:
			static void Init();

			static void ReadVendorID();

			static unsigned int getSteppingID();
			static unsigned int getModel();
			static unsigned int getFamily();
			static unsigned int getProcessorType();
			static unsigned int getExtendedModelID();
			static unsigned int getExtendedFamilyID();

			//ECX
			static bool hasSSE3();
			static bool hasMONITOR();
			static bool hasDSCPL();
			static bool hasEST();
			static bool hasTM2();
			static bool hasCNXTID();
			static bool hasCMPXCHG16B();

			//EDX
			static bool hasFPU();
			static bool hasVME();
			static bool hasDE();
			static bool hasPSE();
			static bool hasTSC();
			static bool hasMSR();
			static bool hasPAE();
			static bool hasMCE();
			static bool hasCX8();
			static bool hasAPIC();
			static bool hasSEP();
			static bool hasMTRR();
			static bool hasPGE();
			static bool hasMCA();
			static bool hasCMOV();
			static bool hasPAT();
			static bool hasPSE36();
			static bool hasPSN();
			static bool hasCLFSH();
			static bool hasDS();
			static bool hasACPI();
			static bool hasMMX();
			static bool hasFXSR();
			static bool hasSSE();
			static bool hasSSE2();
			static bool hasSS();
			static bool hasHTT();
			static bool hasTM();
			static bool hasPBE();

			static void PrintCPUInfo();
		private:
			static dword CPUID0;
			static dword CPUID1;
			static dword CPUID2;
			static char VendorID[13];
			static char BrandString[49];
			static void ReadCPUIDInfo();
			static void ReadBrandString();
		};
	}
}

#endif
