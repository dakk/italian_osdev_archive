/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: cpuid.cpp			|
	| Date: 10/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/cpuid.h>

#include <kservices/kout.h>

#include <hardware/cpu/bitops.h>

//Tipi definiti
#include <kdef.h>



/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::cpu;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

//Vendor ID
char CPUID::VendorID[13];
char CPUID::BrandString[49];
dword CPUID::CPUID0;
dword CPUID::CPUID1;
dword CPUID::CPUID2;

/*
  Inizializza la classe leggendo le informazioni dal processore
*/
void CPUID::Init(){
	//Legge e stampa a video il VendorID
	kservices::KOut::Write("CPUID: VendorID: ");
	ReadVendorID();
	kservices::KOut::WriteLine(VendorID);

	//Legge le informazioni restituite da CPUID (con eax = 1)
	ReadCPUIDInfo();

	ReadBrandString();
}

void CPUID::ReadCPUIDInfo(){
	asm volatile("movl $1, %eax\n");
	asm("cpuid");
	asm volatile("movl %%edx ,%0": "=r"(CPUID0));
	asm volatile("movl %%ecx ,%0": "=r"(CPUID1));
	asm volatile("movl %%eax ,%0": "=r"(CPUID2));
}

/*
  Legge il vendor ID
*/
void CPUID::ReadVendorID(){
	dword regebx = 0;
	dword regecx = 0;
	dword regedx = 0;

	asm volatile("movl $0, %eax\n");
	asm("cpuid");
	asm volatile("movl %%ebx ,%0": "=r"(regebx));
	asm volatile("movl %%ecx ,%0": "=r"(regecx));
	asm volatile("movl %%edx ,%0": "=r"(regedx));
	
	VendorID[0] = (char) (regebx & 0x000000FF);
	VendorID[1] = (char) ((regebx & 0x0000FF00) >> 8);
	VendorID[2] = (char) ((regebx & 0x00FF0000) >> 16);
	VendorID[3] = (char) ((regebx & 0xFF000000) >> 24);

	VendorID[4] = (char) (regedx & 0x000000FF);
	VendorID[5] = (char) ((regedx & 0x0000FF00) >> 8);
	VendorID[6] = (char) ((regedx & 0x00FF0000) >> 16);
	VendorID[7] = (char) ((regedx & 0xFF000000) >> 24);

	VendorID[8] = (char) (regecx & 0x000000FF);
	VendorID[9] = (char) ((regecx & 0x0000FF00) >> 8);
	VendorID[10] = (char) ((regecx & 0x00FF0000) >> 16);
	VendorID[11] = (char) ((regecx & 0xFF000000) >> 24);

	VendorID[12] = '\0';
}

void CPUID::ReadBrandString(){
	//In ogni registro vengono salvati 4 caratteri
	volatile dword regeax;
	volatile dword regebx;
	volatile dword regecx;
	volatile dword regedx;


	//Leggo i primi 16 caratteri
	asm volatile("movl $0x80000002, %eax\n");
	asm("cpuid");
	//Salvo il contenuto dei registri
	asm volatile("movl %%eax ,%0": "=r"(regeax));
	asm volatile("movl %%ebx ,%0": "=r"(regebx));
	asm volatile("movl %%ecx ,%0": "=r"(regecx));
	asm volatile("movl %%edx ,%0": "=r"(regedx));

	//Leggo i caratteri contenuti in eax
	BrandString[0] = (char) (regeax & 0x000000FF);
	BrandString[1] = (char) ((regeax & 0x0000FF00) >> 8);
	BrandString[2] = (char) ((regeax & 0x00FF0000) >> 16);
	BrandString[3] = (char) ((regeax & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ebx
	BrandString[4] = (char) (regebx & 0x000000FF);
	BrandString[5] = (char) ((regebx & 0x0000FF00) >> 8);
	BrandString[6] = (char) ((regebx & 0x00FF0000) >> 16);
	BrandString[7] = (char) ((regebx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ecx
	BrandString[8] = (char) (regecx & 0x000000FF);
	BrandString[9] = (char) ((regecx & 0x0000FF00) >> 8);
	BrandString[10] = (char) ((regecx & 0x00FF0000) >> 16);
	BrandString[11] = (char) ((regecx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in edx
	BrandString[12] = (char) (regedx & 0x000000FF);
	BrandString[13] = (char) ((regedx & 0x0000FF00) >> 8);
	BrandString[14] = (char) ((regedx & 0x00FF0000) >> 16);
	BrandString[15] = (char) ((regedx & 0xFF000000) >> 24);


	//Leggo i prossimi 16 caratteri
	asm volatile("movl $0x80000003, %eax\n");
	asm("cpuid");
	//Salvo il contenuto dei registri
	asm volatile("movl %%eax ,%0": "=r"(regeax));
	asm volatile("movl %%ebx ,%0": "=r"(regebx));
	asm volatile("movl %%ecx ,%0": "=r"(regecx));
	asm volatile("movl %%edx ,%0": "=r"(regedx));

	//Leggo i caratteri contenuti in eax
	BrandString[16] = (char) (regeax & 0x000000FF);
	BrandString[17] = (char) ((regeax & 0x0000FF00) >> 8);
	BrandString[18] = (char) ((regeax & 0x00FF0000) >> 16);
	BrandString[19] = (char) ((regeax & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ebx
	BrandString[20] = (char) (regebx & 0x000000FF);
	BrandString[21] = (char) ((regebx & 0x0000FF00) >> 8);
	BrandString[22] = (char) ((regebx & 0x00FF0000) >> 16);
	BrandString[23] = (char) ((regebx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ecx
	BrandString[24] = (char) (regecx & 0x000000FF);
	BrandString[25] = (char) ((regecx & 0x0000FF00) >> 8);
	BrandString[26] = (char) ((regecx & 0x00FF0000) >> 16);
	BrandString[27] = (char) ((regecx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in edx
	BrandString[28] = (char) (regedx & 0x000000FF);
	BrandString[29] = (char) ((regedx & 0x0000FF00) >> 8);
	BrandString[30] = (char) ((regedx & 0x00FF0000) >> 16);
	BrandString[31] = (char) ((regedx & 0xFF000000) >> 24);


	//Leggo gli ultimi 16 caratteri
	asm volatile("movl $0x80000004, %eax\n");
	asm("cpuid");
	//Salvo il contenuto dei registri
	asm volatile("movl %%eax ,%0": "=r"(regeax));
	asm volatile("movl %%ebx ,%0": "=r"(regebx));
	asm volatile("movl %%ecx ,%0": "=r"(regecx));
	asm volatile("movl %%edx ,%0": "=r"(regedx));

	//Leggo i caratteri contenuti in eax
	BrandString[32] = (char) (regeax & 0x000000FF);
	BrandString[33] = (char) ((regeax & 0x0000FF00) >> 8);
	BrandString[34] = (char) ((regeax & 0x00FF0000) >> 16);
	BrandString[35] = (char) ((regeax & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ebx
	BrandString[36] = (char) (regebx & 0x000000FF);
	BrandString[37] = (char) ((regebx & 0x0000FF00) >> 8);
	BrandString[38] = (char) ((regebx & 0x00FF0000) >> 16);
	BrandString[39] = (char) ((regebx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in ecx
	BrandString[40] = (char) (regecx & 0x000000FF);
	BrandString[41] = (char) ((regecx & 0x0000FF00) >> 8);
	BrandString[42] = (char) ((regecx & 0x00FF0000) >> 16);
	BrandString[43] = (char) ((regecx & 0xFF000000) >> 24);

	//Leggo i caratteri contenuti in edx
	BrandString[44] = (char) (regedx & 0x000000FF);
	BrandString[45] = (char) ((regedx & 0x0000FF00) >> 8);
	BrandString[46] = (char) ((regedx & 0x00FF0000) >> 16);
	BrandString[47] = (char) ((regedx & 0xFF000000) >> 24);

	//Aggiungo il carattere terminatore della stringa
	BrandString[48] = '\0';

	//Stampo a video la BrandString
	kservices::KOut::Write("CPUID: BrandString:");
	kservices::KOut::WriteLine(BrandString);
}


unsigned int CPUID::getSteppingID(){
	return (CPUID2 & 0x0000000F);
}

unsigned int CPUID::getModel(){
	return ((CPUID2 & 0x000000F0) >> 4);
}

unsigned int CPUID::getFamily(){
	return ((CPUID2 & 0x00000F00) >> 8);
}

unsigned int CPUID::getProcessorType(){
	return ((CPUID2 & 0x00003000) >> 12);
}

unsigned int CPUID::getExtendedModelID(){
	return ((CPUID2 & 0x000F0000) >> 16);
}

unsigned int CPUID::getExtendedFamilyID(){
	return ((CPUID2 & 0x0FF00000) >> 20);
}

bool CPUID::hasFPU(){
	if(test_bit(0, &CPUID0)) return true;
	return false;
}

bool CPUID::hasVME(){
	if(test_bit(1, &CPUID0)) return true;
	return false;
}

bool CPUID::hasDE(){
	if(test_bit(2, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPSE(){
	if(test_bit(3, &CPUID0)) return true;
	return false;
}

bool CPUID::hasTSC(){
	if(test_bit(4, &CPUID0)) return true;
	return false;
}

bool CPUID::hasMSR(){
	if(test_bit(5, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPAE(){
	if(test_bit(6, &CPUID0)) return true;
	return false;
}

bool CPUID::hasMCE(){
	if(test_bit(7, &CPUID0)) return true;
	return false;
}

bool CPUID::hasCX8(){
	if(test_bit(8, &CPUID0)) return true;
	return false;
}

bool CPUID::hasAPIC(){
	if(test_bit(9, &CPUID0)) return true;
	return false;
}

bool CPUID::hasSEP(){
	if(test_bit(11, &CPUID0)) return true;
	return false;
}

bool CPUID::hasMTRR(){
	if(test_bit(12, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPGE(){
	if(test_bit(13, &CPUID0)) return true;
	return false;

}

bool CPUID::hasMCA(){
	if(test_bit(14, &CPUID0)) return true;
	return false;
}

bool CPUID::hasCMOV(){
	if(test_bit(15, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPAT(){
	if(test_bit(16, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPSE36(){
	if(test_bit(17, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPSN(){
	if(test_bit(18, &CPUID0)) return true;
	return false;
}

bool CPUID::hasCLFSH(){
	if(test_bit(19, &CPUID0)) return true;
	return false;
}

bool CPUID::hasDS(){
	if(test_bit(21, &CPUID0)) return true;
	return false;
}

bool CPUID::hasACPI(){
	if(test_bit(22, &CPUID0)) return true;
	return false;
}

bool CPUID::hasMMX(){
	if(test_bit(23, &CPUID0)) return true;
	return false;
}

bool CPUID::hasFXSR(){
	if(test_bit(24, &CPUID0)) return true;
	return false;
}

bool CPUID::hasSSE(){
	if(test_bit(25, &CPUID0)) return true;
	return false;
}

bool CPUID::hasSSE2(){
	if(test_bit(26, &CPUID0)) return true;
	return false;
}

bool CPUID::hasSS(){
	if(test_bit(27, &CPUID0)) return true;
	return false;
}

bool CPUID::hasHTT(){
	if(test_bit(28, &CPUID0)) return true;
	return false;
}

bool CPUID::hasTM(){
	if(test_bit(29, &CPUID0)) return true;
	return false;
}

bool CPUID::hasPBE(){
	if(test_bit(31, &CPUID0)) return true;
	return false;
}

bool CPUID::hasSSE3(){
	if(test_bit(0, &CPUID1)) return true;
	return false;
}

bool CPUID::hasMONITOR(){
	if(test_bit(3, &CPUID1)) return true;
	return false;
}

bool CPUID::hasDSCPL(){
	if(test_bit(4, &CPUID1)) return true;
	return false;
}

bool CPUID::hasEST(){
	if(test_bit(7, &CPUID1)) return true;
	return false;
}

bool CPUID::hasTM2(){
	if(test_bit(8, &CPUID1)) return true;
	return false;
}

bool CPUID::hasCNXTID(){
	if(test_bit(10, &CPUID1)) return true;
	return false;
}

bool CPUID::hasCMPXCHG16B(){
	if(test_bit(13, &CPUID1)) return true;
	return false;
}

void CPUID::PrintCPUInfo(){
	kservices::KOut::WriteLine(VendorID);
	kservices::KOut::WriteLine(BrandString);

	kservices::KOut::Write("Stepping ID:");
	kservices::KOut::WriteLine(getSteppingID());
	
	kservices::KOut::Write("Model:");
	kservices::KOut::WriteLine(getModel());

	kservices::KOut::Write("Family:");
	kservices::KOut::WriteLine(getFamily());

	kservices::KOut::Write("Processor Type:");
	kservices::KOut::WriteLine(getProcessorType());

	kservices::KOut::Write("Extended Model ID:");
	kservices::KOut::WriteLine(getExtendedModelID());

	kservices::KOut::Write("Extended Family ID:");
	kservices::KOut::WriteLine(getExtendedModelID());

	//Stampa le flags a video
	kservices::KOut::Write("Flags: ");
	if (hasFPU()) kservices::KOut::Write("FPU ");
	if (hasVME()) kservices::KOut::Write("VME ");
	if (hasDE()) kservices::KOut::Write("DE ");
	if (hasPSE()) kservices::KOut::Write("PSE ");
	if (hasTSC()) kservices::KOut::Write("TSC ");
	if (hasMSR()) kservices::KOut::Write("MSR ");
	if (hasPAE()) kservices::KOut::Write("PAE ");
	if (hasMCE()) kservices::KOut::Write("MCE ");
	if (hasCX8()) kservices::KOut::Write("CX8 ");
	if (hasAPIC()) kservices::KOut::Write("APIC ");
	if (hasSEP()) kservices::KOut::Write("SEP ");
	if (hasMTRR()) kservices::KOut::Write("MTRR ");
	if (hasPGE()) kservices::KOut::Write("PGE ");
	if (hasMCA()) kservices::KOut::Write("MCA ");
	if (hasCMOV()) kservices::KOut::Write("CMOV ");
	if (hasPAT()) kservices::KOut::Write("PAT ");
	if (hasPSE36()) kservices::KOut::Write("PSE36 ");
	if (hasPSN()) kservices::KOut::Write("PSN ");
	if (hasCLFSH()) kservices::KOut::Write("CLFSH ");
	if (hasDS()) kservices::KOut::Write("DS ");
	if (hasACPI()) kservices::KOut::Write("ACPI ");
	if (hasMMX()) kservices::KOut::Write("MMX ");
	if (hasFXSR()) kservices::KOut::Write("FXSR ");
	if (hasSSE()) kservices::KOut::Write("SSE ");
	if (hasSSE2()) kservices::KOut::Write("SSE2 ");
	if (hasSS()) kservices::KOut::Write("SS ");
	if (hasHTT()) kservices::KOut::Write("HTT ");
	if (hasTM()) kservices::KOut::Write("TM ");
	if (hasPBE()) kservices::KOut::Write("PBE ");
	if (hasSSE3()) kservices::KOut::Write("SSE3 ");
	if (hasMONITOR()) kservices::KOut::Write("MONITOR ");
	if (hasDSCPL()) kservices::KOut::Write("DSCPL ");
	if (hasEST()) kservices::KOut::Write("EST ");
	if (hasTM2()) kservices::KOut::Write("TM2 ");
	if (hasCNXTID()) kservices::KOut::Write("CNXTID ");
	if (hasCMPXCHG16B()) kservices::KOut::Write("CMPXCHG16B");
	kservices::KOut::WriteLine();
}
