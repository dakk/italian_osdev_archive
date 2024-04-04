/*!	\file arch/i386/kernel/cpu.c
 *	\brief Intel & AMD CPU features.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-24
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <string.h>
#include <stdlib.h>
#include <types.h>
#include <arch/bitops.h>
#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/mmx.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/time.h>

#include <arch/cpu.h>

//! Global structure for CPU informations.
cpuinfo_t cpu;

//! All the string names for Intel CPU models.
static cpu_name_t intel_cpu_names[] =
{
	{
		.family = 4, .model_names =
		{
			[ 0 ] = "486 DX-25/33",
			[ 1 ] = "486 DX-50",
			[ 2 ] = "486 SX",
			[ 3 ] = "486 DX/2",
			[ 4 ] = "486 SL",
			[ 5 ] = "486 SX/2",
			[ 7 ] = "486 DX/2-WB",
			[ 8 ] = "486 DX/4",
			[ 9 ] = "486 DX/4-WB",
		}
	},
	{
		.family = 5, .model_names =
		{
			[ 0 ] = "Pentium 60/66 A-step",
			[ 1 ] = "Pentium 60/66",
			[ 2 ] = "Pentium 75-200",
			[ 3 ] = "Overdrive PODP5V83",
			[ 4 ] = "Pentium MMX",
			[ 7 ] = "Mobile Pentium 75-200",
			[ 8 ] = "Mobile Pentium MMX",
		}
	},
	{
		.family = 6, .model_names =
		{
			[ 0  ] = "Pentium Pro A-step",
			[ 1  ] = "Pentium Pro",
			[ 3  ] = "Pentium II (Klamath)",
			[ 4  ] = "Pentium II (Deschutes)",
			[ 5  ] = "Pentium II (Deschutes)",
			[ 6  ] = "Mobile Pentium II",
			[ 7  ] = "Pentium III (Katmai)",
			[ 8  ] = "Pentium III (Coppermine)",
			[ 10 ] = "Pentium III (Cascades)",
			[ 11 ] = "Pentium III (Tualatin)",
		}
	},
	{
		.family = 15, .model_names =
		{
			[ 0 ] = "Pentium 4 (Unknown)",
			[ 1 ] = "Pentium 4 (Willamette)",
			[ 2 ] = "Pentium 4 (Northwood)",
			[ 4 ] = "Pentium 4 (Foster)",
			[ 5 ] = "Pentium 4 (Foster)",
		}
	},
};

//! All the string names for AMD CPU models.
static cpu_name_t amd_cpu_names[] =
{
	{
		.family = 4, .model_names =
		{
			[ 3  ] = "486 DX/2",
			[ 7  ] = "486 DX/2-WB",
			[ 8  ] = "486 DX/4",
			[ 9  ] = "486 DX/4-WB",
			[ 14 ] = "Am5x86-WT",
			[ 15 ] = "Am5x86-WB",
		}
	},
	{
		.family = 5, .model_names =
		{
			[ 0 ] = "K5/SSA5",
			[ 1 ] = "K5",
			[ 2 ] = "K5",
			[ 3 ] = "K5",
			[ 6 ] = "K6",
			[ 7 ] = "K6",
			[ 8 ] = "K6-2",
			[ 9 ] = "K6-3",
		}
	},
	{
		.family = 6, .model_names =
		{
			[ 0 ] = "Athlon",
			[ 1 ] = "Athlon",
			[ 2 ] = "Athlon",
			[ 4 ] = "Athlon",
		}
	},
};

//! Feature flags for Intel CPUs (edx register).
static char *intel_flags_edx[] =
{
	"fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
	"cx8", "apic", NULL, "sep", "mtrr", "pge", "mca", "cmov",
	"pat", "pse36", "pn", "clflush", NULL, "ds", "acpi", "mmx",
	"fxsr", "sse", "sse2", "ss", "htt", "tm", "ia64", "sbf",
};

//! Feature flags for Intel CPUs (exc register).
static char *intel_flags_ecx[] =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, "tm2",
	"est", NULL, "cid", NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

//! Feature flags for AMD CPUs.
static char *amd_flags[] =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, "syscall", NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, "mmxext", NULL,
	NULL, NULL, NULL, NULL, NULL, "lm", "3dnowext", "3dnow",
};

//! Cache TLB informations
static cache_info_t cache_strings[] =
{
	{ 0x01, "Instruction TLB: 4KB Pages 4-way set associative, 32 entries" },
	{ 0x02, "Instruction TLB: 4MB Pages, 4-way set associative, 2 entries" },
	{ 0x03, "Data TLB: 4KB Pages, 4-way set associative, 64 entries" },
	{ 0x04, "Data TLB: 4MB Pages, 4-way set associative, 8 entries" },
	{ 0x06, "1st-level instruction cache: 8KB, 4-way set associative, 32 byte line size" },
	{ 0x08, "1st-level instruction cache: 16KB, 4-way set associative, 32 byte line size" },
	{ 0x0A, "1st-level data cache: 8KB, 2-way set associative, 32 byte line size" },
	{ 0x0C, "1st-level data cache: 16KB, 4-way set associative, 32 byte line size" },
	{ 0x22, "3rd-level cache: 512KB, 4-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x23, "3rd-level cache: 1MB, 8-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x40, "No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache" },
	{ 0x41, "2nd-level cache: 128KB, 4-way set associative, 32 byte line size" },
	{ 0x42, "2nd-level cache: 256KB, 4-way set associative, 32 byte line size" },
	{ 0x43, "2nd-level cache: 512KB, 4-way set associative, 32 byte line size" },
	{ 0x44, "2nd-level cache: 1MB, 4-way set associative, 32 byte line size" },
	{ 0x45, "2nd-level cache: 2MB, 4-way set associative, 32 byte line size" },
	{ 0x50, "Instruction TLB: 4KB and 2MB or 4MB pages, 64 entries" },
	{ 0x51, "Instruction TLB: 4KB and 2MB or 4MB pages, 128 entries" },
	{ 0x52, "Instruction TLB: 4KB and 2MB or 4MB pages, 256 entries" },
	{ 0x5B, "Data TLB: 4KB and 4MB pages, 64 entries" },
	{ 0x5C, "Data TLB: 4KB and 4MB pages, 128 entries" },
	{ 0x5D, "Data TLB: 4KB and 4MB pages, 256 entries" },
	{ 0x66, "1st-level data cache: 8KB, 4-way set associative, 64 byte line size" },
	{ 0x67, "1st-level data cache: 16KB, 4-way set associative, 64 byte line size" },
	{ 0x68, "1st-level data cache: 32KB, 4-way set associative, 64 byte line size" },
	{ 0x70, "Trace cache: 12K-op, 8-way set associative" },
	{ 0x71, "Trace cache: 16K-op, 8-way set associative" },
	{ 0x72, "Trace cache: 32K-op, 8-way set associative" },
	{ 0x79, "2nd-level cache: 128KB, 8-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x7A, "2nd-level cache: 256KB, 8-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x7B, "2nd-level cache: 512KB, 8-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x7C, "2nd-level cache: 1MB, 8-way set associative, dual-sectored line, 64 byte sector size" },
	{ 0x82, "2nd-level cache: 256KB, 8-way set associative, 32 byte line size" },
	{ 0x83, "2nd-level cache: 512KB, 8-way set associative, 32 byte line size" },
	{ 0x84, "2nd-level cache: 1MB, 8-way set associative, 32 byte line size" },
	{ 0x85, "2nd-level cache: 2MB, 8-way set associative, 32 byte line size" },
};

//! Check if the 'cpuid' instruction is available.
//! This is true if the cpuid detection flag in EFLAGS register
//! is changeable.
static __INLINE__ int cpuid_is_available()
{
	int flag1, flag2;

	__asm__ __volatile__ (
		"pushfl\n"
		"pushfl\n"
		"popl %0\n"
		"movl %0, %1\n"
		"xorl %2, %0\n"
		"pushl %0\n"
		"popfl\n"
		"pushfl\n"
		"popl %0\n"
		"popfl\n"
		: "=r"(flag1), "=r"(flag2) : "i"(EFLAGS_ID)
	);
	return( (flag1^flag2) & EFLAGS_ID );
}

//! Check if the processor is a 486 or higher.
//! This is true if the alignement check bit in EFLAGS register
//! is changeable.
static __INLINE__ int cpu_is_486()
{
	int flag1, flag2;

	__asm__ __volatile__ (
		"pushfl\n"
		"pushfl\n"
		"popl %0\n"
		"movl %0, %1\n"
		"xorl %2, %0\n"
		"pushl %0\n"
		"popfl\n"
		"pushfl\n"
		"popl %0\n"
		"popfl\n"
		: "=r"(flag1), "=r"(flag2) : "i"(EFLAGS_AC)
	);
	return( (flag1^flag2) & EFLAGS_AC );
}

//! Evaluate and return the CPU frequency in kHz.
void cpu_evaluate_frequency( cpuinfo_t *cpu )
{
	uint64_t timestamp1, timestamp2;
	uint32_t flags;

	if( !cpu->feature.flags.edx.tsc )
	{
		// CPU doesn't support Time Stamp Counter!
		// Cannot evaluate the frequency.
		cpu->frequency = 0;
		return;
	}

	local_irq_save( flags );

	// Wait until the second has precisely just started.
	(void)sys_time( NULL );
	// Read the value of the current time-stamp counter (64-bit).
	rdtscll( timestamp1 );
	// Wait exactly one second.
	(void)sys_time( NULL );
	// Read the value of the current time-stamp counter again.
	rdtscll( timestamp2 );
	// Now we can estimate the CPU frequency as follows.
	cpu->frequency = ((uint32_t)(timestamp2 - timestamp1)/1000);

	local_irq_restore( flags );
}

//! Get the cpu brand string if it is available.
int cpu_get_brandstring( cpuinfo_t *cpu )
{
	unsigned int *s = (unsigned int *)cpu->name;
	char *p, *q;
	uint32_t dummy, eax;

	// Check if brand string is available.
	cpuid( CPU_BRAND_STRING_IS_AVAILABLE,
		&eax, &dummy, &dummy, &dummy);
	if( eax<0x80000004 )
	{
		// Brand string not supported! Zero the cpu name.
		memset( cpu->name, 0, sizeof(cpu->name) );
		return( -1 );
	}

	// Get the cpu brand string.
	cpuid( CPU_GET_BRAND_STRING1, &s[0], &s[1], &s[2], &s[3] );
	cpuid( CPU_GET_BRAND_STRING2, &s[4], &s[5], &s[6], &s[7] );
	cpuid( CPU_GET_BRAND_STRING3, &s[8], &s[9], &s[10], &s[11] );
	cpu->name[48] = '\0';

	// Intel chips right justify the string.
	// Left justify this string.
	p = q = &cpu->name[0];
	while ( *p == ' ' )
		p++;
	if ( p != q )
	{
		while ( *p )
			*q++ = *p++;
		// Zero pad the rest.
		while ( q <= &cpu->name[48] )
			*q++ = '\0';
	}

	// Well done.
	return( 0 );
}

//! Get the cpu model name.
int cpu_get_modelname( cpuinfo_t *cpu )
{
	cpu_name_t *names;
	int count, i;

	// First of all try to get the brand string.
	if( cpu_get_brandstring(cpu)==0 )
	{
		// Well done, brand string supported.
		return( 0 );
	}

	// Find the vendor of the CPU.
	if( strcmp(cpu->vendor.string, "GenuineIntel")==0 )
	{
		// Intel CPU name.
		// Try to identify the cpu name with the brand ID.
		// This is useful to identify Celeron and Xeon
		// processor.
		switch( cpu->brand.id )
		{
			case 1:
			case 10:
				strncpy( cpu->name,
					"Intel(R) Celeron(R)",
					sizeof(cpu->name) );
				return( 0 );
			break;

			case 3:
				if( cpu->signature.num==0x6b1 )
					strncpy( cpu->name,
						"Intel(R) Celeron(R)",
						sizeof(cpu->name) );
				else
					strncpy( cpu->name,
						"Intel(R) Pentium(R) Xeon",
						sizeof(cpu->name) );
				return( 0 );
			break;

			case 6:
				strncpy( cpu->name,
					"Mobile Intel(R) Pentium(R) III",
					sizeof(cpu->name) );
				return( 0 );
			break;

			case 7:
			case 15:
				strncpy( cpu->name,
					"Mobile Intel(R) Celeron(R)",
					sizeof(cpu->name) );
				return( 0 );
			break;

			case 11:
				if( cpu->signature.num<0xf13 )
					strncpy( cpu->name,
						"Intel(R) Xeon MP",
						sizeof(cpu->name) );
				else
					strncpy( cpu->name,
						"Intel(R) Xeon",
						sizeof(cpu->name) );
				return( 0 );
			break;

			case 12:
				strncpy( cpu->name,
					"Intel(R) Xeon MP",
					sizeof(cpu->name) );
				return( 0 );
			break;

			case 13:
				if( cpu->signature.num<0xf13 )
					strncpy( cpu->name,
						"Intel(R) Xeon",
						sizeof(cpu->name) );
				else
					strncpy( cpu->name,
						"Mobile Intel(R) Pentium 4",
						sizeof(cpu->name) );
			break;
		}
		// Find the cpu name in the intel list.
		names = intel_cpu_names;
		count = _countof( intel_cpu_names );
	}
	else if( strcmp(cpu->vendor.string, "AuthenticAMD")==0 )
	{
		// AMD CPU name.
		// Find the cpu name in the amd list.
		names = amd_cpu_names;
		count = _countof( amd_cpu_names );
	}
	else
	{
		// Copy the unknown string to the cpu name and vendor string.
		strncpy( cpu->vendor.string, "Unknown", sizeof(cpu->vendor.string) );
		switch( cpu->signature.flags.family )
		{
			case 3:
			strncpy( cpu->name, "386", sizeof(cpu->name) );
			break;

			case 4:
			strncpy( cpu->name, "486", sizeof(cpu->name) );
			break;

			default:
			strncpy( cpu->name, "Unknown", sizeof(cpu->name) );
			break;
		}
		return( -1 );
	}

	// Get the right CPU model name.
	for( i=0; i<count; i++ )
	{
		if( cpu->signature.flags.family==names[i].family )
		{
			int id = cpu->signature.flags.model;
			// Copy the cpu name.
			strncpy( cpu->name,
				names[i].model_names[ id ],
				sizeof(cpu->name) );
		}
	}

	return( 0 );
}

//! Get the CPU informations and fill the cpuinfo_t * structure.
int cpu_get_info( cpuinfo_t *cpu )
{
	uint32_t dummy;

	// Reset the structure.
	memset( cpu, 0, sizeof(cpuinfo_t) );

	// Check if 'cpuid' instruction is available.
	if( !cpuid_is_available() )
	{
		if( !cpu_is_486() )
		{
			// 80386 detected.
			cpu->signature.flags.family = 3;
		}
		else
		{
			// 80486 detected.
			cpu->signature.flags.family = 4;
		}
	}
	else
	{
		// Execute the 'cpuid' instruction command #0.
		cpuid( GET_CPU_VENDOR,
			&dummy, &cpu->vendor.num[0],
			&cpu->vendor.num[2], &cpu->vendor.num[1] );

		// Execute the 'cpuid' instruction command #1.
		cpuid( GET_CPU_INFO,
			&cpu->signature.num, &cpu->brand.num,
			&cpu->feature.num[0], &cpu->feature.num[1] );

		// Evaluate the frequency if it is possible.
		cpu_evaluate_frequency( cpu );
	}

	// Get the model name.
	cpu_get_modelname( cpu );

	// Well done.
	return( 0 );
}

//! Dump CPU internal cache informations.
void cpu_dump_cache_info()
{
	uint32_t cache_info[4];
	char *desc;
	int i, j, n;
	uint32_t dummy;

	// Get the number of times to iterate (lower 8-bits).
	cpuid( GET_CPU_CACHE, &n, &dummy, &dummy, &dummy );
	n &= 0xff;
	for( i=0; i<n; i++ )
	{
		cpuid( GET_CPU_CACHE,
			&cache_info[0], &cache_info[1],
			&cache_info[2], &cache_info[3] );
	}

	// If bit 31 is set this is an unknown format.
	for( i=0; i<3; i++ )
		if ( (cache_info[i] & 0x80000000) )
			cache_info[i] = 0;

	// Look up in the cache strings table.
	desc = (char *)cache_info;
	// Byte 0 is the level count, not a descriptor.
	for( i=1; i<4*sizeof(uint32_t); i++ )
	{
		if ( desc[i] )
		{
			for( j = 0; j < _countof(cache_strings); j++ )
			{
				if ( desc[i]==cache_strings[j].index )
				{
					kprintf( "%s\n", cache_strings[j].string );
					break;
				}
			}
		}
	}
}

//! Show a brief description of the cpu.
void show_brief_cpuinfo()
{
	kprintf( "Vendor ID          : %s\n", cpu.vendor.string );
	kprintf( "Name               : %s\n", cpu.name );
	if( cpu.frequency )
	{
		kprintf( "Frequency          : %u.%u MHz\n",
			cpu.frequency/1000, cpu.frequency%1000 );
	}
	else
	{
		kprintf( "Frequency          : ??? MHz\n" );
	}
	kprintf( "Family             : %u\n"
		 "Model              : %u\n"
		 "Stepping ID        : %u\n"
		 "Extended Family    : %u\n"
		 "Extended Model     : %u\n",
		cpu.signature.flags.family,
		cpu.signature.flags.model,
		cpu.signature.flags.stepping,
		cpu.signature.flags.ext_family,
		cpu.signature.flags.ext_model );
}

//! Dump all the CPU informations to standard output.
void show_cpuinfo()
{
	// Show a brief description of the cpu.
	show_brief_cpuinfo();

	// Show the BogoMIPS evaluation.
	kprintf( "BogoMIPS           : %lu.%02lu\n",
		cpu.loops_per_tick / (500000 / HZ),
		(cpu.loops_per_tick / (5000 / HZ)) % 100 );

	// Print feature flags.
	kprintf( "Feature Flags      : \n" );
	if( strcmp(cpu.vendor.string, "GenuineIntel")==0 )
	{
		// Intel cpu.
		int i;
		// Test each bit of the feature flags.
		for( i=0; i<32; i++ )
			if( test_bit(i, &cpu.feature.flags.edx) )
				kprintf( "%s ", intel_flags_edx[i] );
		for( i=0; i<32; i++ )
			if( test_bit(i, &cpu.feature.flags.ecx) )
				kprintf( "%s ", intel_flags_ecx[i] );
	}
	else if( strcmp(cpu.vendor.string, "GenuineIntel")==0 )
	{
		// AMD cpu.
		int i;
		// Test each bit of the feature flags.
		for( i=0; i<32; i++ )
			if( test_bit(i, &cpu.feature.flags.edx) )
				kprintf( "%s ", amd_flags[i] );
	}
	// Print the BogoMIPS evaluation.
	kprintf( "\n" );

	// Dump internal cache informations.
	kprintf( "Cache informations : \n" );
	cpu_dump_cache_info();
}

//! Initialize the main CPU.
void __INIT__ init_cpu()
{
	cpu_get_info( &cpu );

	printk( KERN_INFO "CPU: Detected '%s' %u.%u MHz processor.\n"
		 "CPU: %s (%d %d %d)\n",
		cpu.vendor.string,
		cpu.frequency/1000, cpu.frequency%1000,
		cpu.name,
		cpu.signature.flags.family,
		cpu.signature.flags.model,
		cpu.signature.flags.stepping );

	if (cpu.feature.flags.edx.fxsr)
	{
		// Set bit 9 of CR4 (the OXFSXR). Setting this bit assumes that the
		// operating system provides facility for saving and restoring the
		// SSE state, using the "fxsave" and "fxrstor" instructions.
		// This is also needed to use SSE or SSE2 processor extensions.
		unsigned int __cr4;
		__asm__ __volatile__ (
			"movl %%cr4, %0\n"
			"or $512, %0\n"
			"movl %0, %%cr4" : "=r"(__cr4));
	}
}
