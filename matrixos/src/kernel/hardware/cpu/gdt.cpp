/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: gdt.cpp			|
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

//Include della GDT
#include <hardware/cpu/gdt.h>

//Include per il debug
#include <kservices/kdbg.h>
#include <kframework/kframework.h>

//Tipi definiti
#include <kdef.h>

//DEBUG: Per abilitare il debug decomentare la seguente riga
//#define GDT_DEBUG

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::cpu;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



//La tabella dei descrittori - qword = 64 bit
qword GDT::gdt[GDT_SIZE];

/*
  Inizzializza la gdt.
*/
void GDT::Init(){

	//Questo segmento è richiesto per il corretto funzionamento della gdt
	gdt[0] = 0;
	
	//Segmento per il codice in lettura ed esecuzione
	AddMemorySegment(0xFFFFF /*limit*/ ,0x00000000 /*base*/,0 /*ring*/,true /*granularity*/,stCodeExecuteRead /*GDTSegmentType*/,true /*bit32*/,true /*SegmentPresent*/);
	
	//Segmento per i dati in lettura e scrittura
	AddMemorySegment(0xFFFFF /*limit*/ ,0x00000000 /*base*/,0 /*ring*/,true /*granularity*/,stDataReadWrite /*GDTSegmentType*/,true /*bit32*/,true /*SegmentPresent*/);

	//Aggiorno il registro
	setGDTR(gdt,GDT_SIZE);
	
	asm("pushl $2; popf");
	
	//l'indice del segmento dei dati è 2, quindi la sua distanza dall'inizio è 0x10 (16 = 8 * 2)
	//mentre l'indice del segmento dei dati è 1 quindi la sua distanza è 8
	asm volatile ("movw $0x10,%ax	\n"
		      "movw %ax,%ds	\n"
		      "movw %ax,%es	\n"
		      "movw %ax,%fs	\n"
		      "movw %ax,%gs	\n"
		      "movw %ax,%ss	\n"
		      "ljmp $0x08,$next	\n"
		      "nop		\n"
		      "nop		\n"
		      "next:		\n");

}

/*
  Aggiunge un descrittore di memoria.
*/
int GDT::AddMemorySegment(int limit, int base, int ring,bool granularity, GDTSegmentType type, bool bit32,bool SegmentPresent){
	
	int index;

	//Variabile temporanea
	qword tmp = 0;
	
	/*
	  Le seguenti righe di codice fanno uso degli operatori 
	  &  - And	1 & 0 = 0	1 & 1 = 1	0 & 0 = 0
	  |  - Or	1 | 0 = 1	1 | 1 = 1	0 | 0 = 0
	  << - Scorrimento a sinistra - b<<n scorre b verso sinistra di n posizioni
	  >> - Scorrimento a destra - b<<n scorre b verso destra di n posizioni
	  --------------------------------------------------------------------------
	  b & 0xE -> Cancella il primo bit di b
	*/
	
	//Limit
	tmp = ((qword)limit) & 0x00000000000FFFFLL;
	tmp = tmp | (((qword)limit) & 0x00000000000F0000LL)<<32;
	
	//Base
	tmp = tmp | (((qword)base) & 0x0000000000FFFFFFLL)<<16;
	tmp = tmp | (((qword)base) & 0x00000000FF000000LL)<<32;

	//Ring
	tmp = tmp | (((qword)ring) & 0x0000000000000003LL)<<46;

	//Granularity
	if (granularity == true) tmp = tmp | 0x0080000000000000LL;

	//Type
	tmp = tmp | (((qword)type) & 0x000000000000000FLL)<<40;
	
	//Descriptor Type
	tmp = tmp | 0x0000100000000000LL;
	
	//Bit32
	if (bit32 == true) tmp = tmp | 0x0040000000000000LL;
	
	//SegmentPresent
	if (SegmentPresent == true) tmp = tmp | 0x0000800000000000LL; 

	//Assegno tmp al primo descrittore libero
	index = getFirstFreeDescriptor();
	
	#ifdef GDT_DEBUG

		kservices::KDbg::Write("gdt.cpp - Aggiungo un descrittore di memoria:");
		kservices::KDbg::Write(" Limit=");
		kservices::KDbg::Write(limit, kframework::HEX);
		
		kservices::KDbg::Write(" Base=");
		kservices::KDbg::Write(base, kframework::HEX);
		
		kservices::KDbg::Write(" SegmentType=");
		kservices::KDbg::Write((int)type, kframework::HEX);
		
		kservices::KDbg::Write(" Index=");
		kservices::KDbg::WriteLine(index);
	#endif
	
	gdt[index] = tmp;
	
	return index;
}

/*
  Cerca il primo descrittore libero.
  Attenzione: In alcuni casi se sono presenti dei descrittori = a 0 il processore si resetta.
  Fare ulteriori verifiche.
  Attenzione: Possibili bug.
*/
int GDT::getFirstFreeDescriptor(){
	int i = 1;

	while(gdt[i] != 0 && i < GDT_SIZE){
		i++;
	}
	
	if(i == GDT_SIZE && gdt[i] != 0) return -1;

	#ifdef GDT_DEBUG
		kservices::KDbg::Write("gdt.cpp - Trovato descrittore libero index=");
		kservices::KDbg::WriteLine(i);
	#endif

	return i;
}

/*
  Elimina un descrittore assegnandoli 0.
  È possibile che assegnare 0 a un descrittore provochi il reset della cpu.
  Attenzione: Possibili bug.
*/
bool GDT::Del(int index){

	#ifdef GDT_DEBUG
		kservices::KDbg::Write("gdt.cpp - Elimino il segmento index=");
		kservices::KDbg::WriteLine(index);
	#endif

	if (index <= GDT_SIZE){
		gdt[index] = 0;
		
		return true;
	}
	return false;
}

void GDT::setGDTR(qword * base, size_t num_desc){
	dword gdt_reg[2];

	gdt_reg[0] = (num_desc * 8) << 16;
	gdt_reg[1] = (dword) base;

	__asm__ __volatile__("lgdt (%0)"::"g"((char *) gdt_reg + 2));

}
