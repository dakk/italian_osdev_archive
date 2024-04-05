
#include	"cpudetect.h"


char cpu_vendor_strng[][KSTRNG_LUNG]=
	{
	"GenuineIntel",
	"AuthenticAMD",
	"CyrixInstead",
	"CentaurHauls",
	"NexGenDriven",
	"GenuineTMx86",
	"RISERISERISE",
	"UMC UMC UMC ",
	"Sconosciuto "
	};

char cpu_map_intel[46]=
	{
	0x40, 0,
	0x41, 1,
	0x42, 2,
	0x43, 3,
	0x44, 4,
	0x45, 5,
	0x47, 6,
	0x48, 7,
	0x49, 8,
	0x50, 9,
	0x51, 10,
	0x52, 11,
	0x53, 12,
	0x54, 13,
	0x57, 14,
	0x58, 15,
	0x60, 16,
	0x61, 17,
	0x63, 18,
	0x65, 29,
	0x66, 20,
	0x67, 21,
	0x68, 22
	};

char cpu_map_amd[44]=
	{
	0x43, 0,
	0x47, 1,
	0x48, 2,
	0x49, 3,
	0x4D, 4,
	0x4E, 5,
	0x50, 6,
	0x51, 7,
	0x52, 7,
	0x53, 7,
	0x56, 8,
	0x57, 8,
	0x58, 9,
	0x59, 10,
	0x5C, 11,
	0x60, 12,
	0x61, 12,
	0x62, 13,
	0x63, 14,
	0x64, 15,
	0x66, 16,
	0x67, 17
	};


char cpu_strng_intel[][KSTRNG_LUNG]=
	{
	"486 DX-25/33",												//4-0
	"486 DX-50",													//4-1
	"486 SX",														//4-2
	"486 DX/2",													//4-3
	"486 SL",														//4-4
	"486 SX/2",													//4-5
	"486 DX/2-WB",												//4-7
	"486 DX/4",													//4-8
	"486 DX/4-WB",												//4-9
	"Pentium 60/66 A-step",											//5-0
	"Pentium 60/66",												//5-1
	"Pentium 75 - 200",												//5-2
	"OverDrive PODP5V83",											//5-3
	"Pentium MMX",												//5-4
	"Mobile Pentium 75 - 200",										//5-7
	"Mobile Pentium MMX",											//5-8
	"Pentium Pro A-step",											//6-0
	"Pentium Pro",													//6-1
	"Pentium II (Klamath)",											//6-3
	"Pentium II (Deschutes), Celeron (Covington), Mobile Pentium II (Dixon)",	//6-5
	"Mobile Pentium II, Celeron (Mendocino)",							//6-6
	"Pentium III (Katmai)",											//6-7
	"Pentium III (Coppermine)"										//6-8
	};

char cpu_strng_amd[][KSTRNG_LUNG]=
	{
	"486 DX/2",			//4-3
	"486 DX/2-WB",		//4-7
	"486 DX/4",			//4-8
	"486 DX/4-WB",		//4-9
	"Am5x86-WT",			//4-14
	"Am5x86-WB",			//4-15
	"K5/SSA5",			//5-0
	"K5",					//5-1.2.3
	"K6",					//5-6.7
	"K6-2",				//5-8
	"K6-3",				//5-9
	"K6-2+ or K6-III+",		//5-13
	"Athlon (25 µm)",		//6-0.1
	"Athlon (18 µm)",		//6-2
	"Duron",				//6-3
	"Athlon (Thunderbird)",	//6-4
	"Athlon (Palamino)",		//6-6
	"Duron (Morgan)"		//6-7
	};


//Rileva il tipo di CPU	scrive in cpumodel	<modello> by <vendor>
void cpu_detect(char* cpumodel)
	{
	int i;
	int signature,step,model,family,type,vendor,found;
	unsigned long flags_prima,flags_dopo;

	union vendorid_t
		{
		int reg[3];
		char string[12];
		} vendorid;

	//Controllo se e' un 386

	asm(
		"pushfl					\n"
		"popl	%%eax			\n"
		" movl	%%eax,%0		\n"
		"xorl		$0x40000, %%eax	\n"
		"pushl	%%eax			\n"
		"popfl					\n"
		"pushfl					\n"
		"popl	%%eax			\n"
		"movl	%%eax,%1		\n"
		"pushl	%0				\n"
		"popfl					\n"
		: "=r" (flags_prima),"=r" (flags_dopo)
		:
		:"%eax"
		);

	flags_prima &=0x40000;
	flags_dopo  &=0x40000;

	if(flags_prima== flags_dopo)
		{
		ksprint(cpumodel,"386 CPU");
		return;
		}

	//Controllo se supporta  CPUID

	asm(
		"pushfl						\n"
		"popl	%%eax				\n"
		"movl	%%eax, %0			\n"
		"xorl		$0x200000, %%eax		\n"
		"pushl	%%eax				\n"
		"popfl						\n"
		"pushfl						\n"
		"popl	%%eax				\n"
		"movl	%%eax, %1			\n"
		"pushl	%0					\n"
		"popfl						\n"
		:"=r"(flags_prima),"=r"(flags_dopo)
		:
		:"%eax"
		);

	flags_prima &= 0x200000;
	flags_dopo  &= 0x200000;

	if(flags_prima==flags_dopo)
		{
		ksprint(cpumodel,"486 CPU w/o CPUID");
		return;
		}

		//E' un 486?

	asm volatile (
				"xor		%%eax,%%eax	\n"
				"movl	$1,%%eax		\n"
				"cpuid					\n"
				:"=a"(signature)
				:
				);

	step=signature & 0xf;
	model=(signature>>4) & 0xf;
	family=(signature>>8) & 0xf;
	type=(signature>>12) & 0x3;

	asm volatile (
				"xor		%%eax,%%eax	\n"
				"cpuid					\n"
				:"=b"(vendorid.reg[0]),"=d"(vendorid.reg[1]),"=c"(vendorid.reg[2])
				:
				);


	vendorid.string[12]='\0';

	vendor=8;//Produttore sconosciuto!
	for(i=0;i<8;i++)
		if(kstrcmp(vendorid.string,cpu_vendor_strng[i])) vendor=i;

	switch(vendor)
		{
		case 0:
			//Cerca nella tabella Intel
			found=-1;
			for(i=0;i<46;i+=2)
				if(cpu_map_intel[i]==(family*16+model)) found=i;

			if (found!=-1)
				ksprint(cpumodel,"%s by %s",cpu_strng_intel[(int)cpu_map_intel[found+1]],cpu_vendor_strng[vendor]);
			else
				ksprint(cpumodel,"Sconosciuto0 by %s",cpu_vendor_strng[vendor]);
		break;

		case 1:
			//Cerca nella tabella AMD
			found=-1;
			for(i=0;i<44;i+=2)
				if(cpu_map_amd[i]==(family*16+model)) found=i;

			if (found!=-1)
				ksprint(cpumodel,"%s by %s",cpu_strng_amd[(int)cpu_map_amd[found+1]],cpu_vendor_strng[vendor]);
			else
				ksprint(cpumodel,"Sconosciuto1 by %s",cpu_vendor_strng[vendor]);
		break;

		case 8:
			ksprint(cpumodel,"Sconosciuto by Sconosciuto!");
		break;

		default:
			ksprint(cpumodel,"Sconosciuto2 by %s",vendorid.string,cpu_vendor_strng[vendor]);

		}

	}
