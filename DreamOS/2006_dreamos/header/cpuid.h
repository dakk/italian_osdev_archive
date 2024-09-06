/*
 * Dreamos
 * cpuid.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __CPUID_H
#define __CPUID_H

// Supported CPUs
#define VENDOR_INTEL_STR			"GenuineIntel"
#define VENDOR_AMD_STR				"AuthenticAMD"
#define VENDOR_CYRIX_STR			"CyrixInstead"
#define VENDOR_CENTAUR_STR		"CentaurHauls"
#define VENDOR_NEXGEN_STR			"NexGenDriven"
#define VENDOR_TRANSMETA_STR	"GenuineTMx86"
#define VENDOR_RISE_STR				"RiseRiseRise"
#define VENDOR_UMC_STR				"UMC UMC UMC"
#define VENDOR_SIS_STR				"SiS SiS SiS"
#define VENDOR_NSC_STR				"Geode by NSC"

enum VENDOR {
	VENDOR_UNKNOWN,
	VENDOR_INTEL,
	VENDOR_AMD,
	VENDOR_CYRIX,
	VENDOR_CENTAUR,
	VENDOR_NEXGEN,
	VENDOR_TRANSMETA,
	VENDOR_RISE,
	VENDOR_UMC,
	VENDOR_SIS,
	VENDOR_NSC
};

/*
 *	CPU Features
 *	This data is set by _kcpuid_1()
 */
typedef struct {
	unsigned long fpu : 1, // Floating Point Unit
		vme : 1, 		// Virtual Mode Extensions
		de : 1, 		// Debugging Extensions
		pse : 1, 		// Page Size Extensions
		tsc : 1, 		// Time Stamp Counter
		msr : 1, 		//	Model-specific registers
		pae : 1, 		// Physical Address Extensions
		mce : 1, 		// Machine Check Exceptions
		cxchg8 : 1, // Compare and exchange 8-byte
		apic : 1, 	// On-chip APIC
		_res1 : 1,	// Reserved
		sep : 1, 		// Fast System Calls
		mtrr : 1, 	// Memory Type Range Registers
		pge : 1, 		// Page Global Enable
		mca : 1, 		// Machine Check Architecture
		cmov : 1, 	// Conditional move instruction
		pat : 1, 		// Page Attribute Table
		pse36 : 1, 	// 36-bit Page Size Extensions
		psn : 1, 		// Processor Serial Number
		clfl : 1, 	// CLFLUSH, Description pending
		_res2 : 1,	// Reserved
		dtes : 1,		// Debug Trace and EMON Store MSRs
		acpi : 1,		// Thermal Control MSR
		mmx : 1,		// MMX instruction set
		fxsr : 1,		// Fast floating point save/restore
		sse : 1,		// SSE (Streaming SIMD Extensions)
		sse2 : 1,		// SSE2 (Streaming SIMD Extensions - #2)
		ss : 1,			// Selfsnoop
		htt : 1,		// Hyper-Threading Technology
		tm1 : 1,		// Thermal Interrupt & Status MSRs.
		ia64 : 1,		// IA-64 (64-bit Intel CPU)
		pbe : 1;		// Pending Break Event
} _kcpu_features;

/*
 * 	CPU Signature.
 * 	This data is set by _kcpuid_1()
 */
typedef struct {
	unsigned long step : 4;
	unsigned long model : 4;
	unsigned long family : 4;
	unsigned long proctype : 2;
	unsigned long _res1 : 2;
	unsigned long extmodel : 4;
	unsigned long extfamily : 8;
	unsigned long _res2 : 4;
} _kcpu_signature;


typedef struct {
	unsigned int family;
	char *models[0x10];
} _kcpu_names;

struct {
	struct {
		int is386;
		int cpuid;
		int cpuid_max;
	} flags;
	struct {
		char *name;
		enum VENDOR id;
	} vendor;
	_kcpu_signature signature;
	_kcpu_features features;
	struct {
		_kcpu_names *family;
		int family_size;
		char *cpu_name;
	} name;
} CPU;

/**
 *   Check CPUID OpCode support
 *   Returns: 1(supported), 0(not supported)
 *   tnx to kernel
 */
int _kcpuid_support(void);

// tnx to kernel
unsigned int _kcpuid_is386();

/**
 *   Return a pointer to CPU Vendor ID String
 */
void _kcpuid_0();

/*
 *	Save signature, features and other
 *	data related to the CPUID 1
 */
void _kcpuid_1();

/**
 *   Check for CPUID support and store informations
 */
void _kcpuid(void);

/*
 *	Print informations about the CPU.
 *	_kcpuid() needs to be called.
 */
void _kcpuid_show();
#endif
