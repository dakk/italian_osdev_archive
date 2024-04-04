/*!	\file include/arch/cpu.h
 *	\brief CPU features.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-23
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef CPU_H
#define CPU_H

//! CPUID operations.
enum
{
	//! Get the cpu vendor string.
	GET_CPU_VENDOR	= 0,
	//! Get cpu informations.
	GET_CPU_INFO	= 1,
	//! Get cpu cache informations.
	GET_CPU_CACHE	= 2,
	//! Check if the brand string is available.
	CPU_BRAND_STRING_IS_AVAILABLE	= 0x80000000,
	//! Get 1st part of the brand string.
	CPU_GET_BRAND_STRING1		= 0x80000002,
	//! Get 1st part of the brand string.
	CPU_GET_BRAND_STRING2		= 0x80000003,
	//! Get 1st part of the brand string.
	CPU_GET_BRAND_STRING3		= 0x80000004,
};

//! The CPU flags mask in register EDX after a CPUID instruction.
typedef struct cpu_flags_edx
{
	unsigned long fpu	: 1;	//!< Floating-point unit on-chip.
	unsigned long vme	: 1;	//!< Virtual mode extension.
	unsigned long de 	: 1;	//!< Debugging extension.
	unsigned long pse	: 1;	//!< Page size extension.
	unsigned long tsc	: 1;	//!< Time stamp counter.
	unsigned long msr	: 1;	//!< Model specific register.
	unsigned long pae	: 1;	//!< Physical address extension.
	unsigned long mce	: 1;	//!< Machine check exception.
	unsigned long cx8	: 1;	//!< cmpxchg8 instruction supported.
	unsigned long apic	: 1;	//!< On-chip APIC hardware support.
	unsigned long _res0	: 1;	//!< Reserved.
	unsigned long sep	: 1;	//!< Fast system call.
	unsigned long mtrr	: 1;	//!< Memory type range register.
	unsigned long pge	: 1;	//!< Page Global enable.
	unsigned long mca	: 1;	//!< Machine check architecture.
	unsigned long cmov	: 1;	//!< Conditional move instruction.
	unsigned long pat	: 1;	//!< Page attribute table.
	unsigned long pse_36	: 1;	//!< 36-bit page size extension.
	unsigned long psn	: 1;	//!< Processor serial number present.
	unsigned long clfsh	: 1;	//!< clflush instruction supported.
	unsigned long _res1	: 1;	//!< Reserved.
	unsigned long ds	: 1;	//!< Debug store.
	unsigned long acpi	: 1;	//!< Thermal monitor and controlled clock support.
	unsigned long mmx	: 1;	//!< MMX technology support.
	unsigned long fxsr	: 1;	//!< Fast floating point save and restore.
	unsigned long sse	: 1;	//!< Streaming SIMD extension support.
	unsigned long sse2	: 1;	//!< Streaming SIMD extension 2 support.
	unsigned long ss	: 1;	//!< Self-snoop.
	unsigned long htt	: 1;	//!< Hyper-threading technology.
	unsigned long tm	: 1;	//!< Thermal monitor supported.
	unsigned long _res2	: 1;	//!< Reserved.
	unsigned long sbf	: 1;	//!< Signal break on ferr.
} __attribute__ ((packed)) cpu_flags_edx_t;

//! The CPU flags mask in register ECX after a CPUID instruction.
typedef struct cpu_flags_ecx
{
	unsigned long _res0	: 7;	//!< Reserved.
	unsigned long tm2	: 1;	//!< Thermal monitor 2.
	unsigned long est	: 1;	//!< Enhanced Speedstep technology.
	unsigned long _res1	: 1;	//!< Reserved.
	unsigned long cid	: 1;	//!< Context ID.
	unsigned long _res2	: 21;	//!< Reserved.
} __attribute__ ((packed)) cpu_flags_ecx_t;

//! The CPU signature mask.
typedef struct cpu_signature
{
	unsigned long stepping	: 4;	//! Stepping ID.
	unsigned long model 	: 4;	//! Model.
	unsigned long family 	: 4;	//! Family.
	unsigned long type 	: 2;	//! Processor type.
	unsigned long _res0 	: 2;	//! Reserved.
	unsigned long ext_model	: 4;	//! Extended Model.
	unsigned long ext_family: 8;	//! Extended Family.
	unsigned long _res1	: 4;	//! Reserved.
} __attribute__ ((packed)) cpu_signature_t;

//! All the CPU informations.
typedef struct cpuinfo
{
	//! The name of the CPU.
	char name[64];
	//! The frequency in kHz.
	uint32_t frequency;
	//! How many loops the CPU does in a clock tick.
	unsigned long loops_per_tick;
	//! Vendor string.
	union
	{
		uint32_t num[4];
		char string[16];
	} vendor;
	//! CPU signature.
	union
	{
		uint32_t num;
		cpu_signature_t flags;
	} signature;
	//! Feature flags.
	union
	{
		uint32_t num[2];
		struct
		{
			cpu_flags_ecx_t ecx;
			cpu_flags_edx_t edx;
		} flags;
	} feature;
	//! Brand ID.
	union
	{
		uint32_t num;
		unsigned long id : 8;
	} brand;
} cpuinfo_t;

//! Intel CPU names.
typedef struct cpu_name
{
	int family;
	char *model_names[16];
} cpu_name_t;

//! The structure for the cache string informations.
typedef struct cache_info
{
	uint8_t index;
	char *string;
} cache_info_t;

//! CPUID detection flag.
#define EFLAGS_ID	0x00200000

//! Alignement check bit.
#define EFLAGS_AC	0x00040000

// --- Prototypes ----------------------------------------------------- //

void show_cpuinfo();
void init_cpu();

#endif
