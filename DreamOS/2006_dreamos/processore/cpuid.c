/*
 * Dreamos
 * cpuid.c
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

#include <cpuid.h>
#include <video.h>
#include <string.h>
#include <stdio.h>
#include <misc.h>
/**
 *   Check CPUID OpCode support
 *   Returns: 1(supported), 0(not supported)
 *   tnx to kernel
 */
int _kcpuid_support(void)
{
/*  int a;

  __asm__ __volatile__
  ("pushfl                  \n"
   "popl %%eax              \n"
   "movl %%eax,%%ecx        \n"
   "xorl $0x200000,%%eax    \n"
   "pushl %%eax             \n"
   "popfl                   \n"
   "pushfl                  \n"
   "popl %%eax              \n"
   "xorl %%ecx,%%eax        \n"
   "je noCPUID              \n"
   "pushl %%ecx             \n"
   "popfl                   \n"
   "movl $1,%%eax           \n"
   "jmp fine2               \n"
   "noCPUID:                \n"
   "movl $0,%%eax           \n"
   "fine2:                  \n"
   "movl %%eax,%0             "
   : "=m" (a)
  );

  return a;
  
  * Replaced with this.
  * If ID is already set, XOR will unset it, so the function will return a false value
  */
  return chk_eflags(EFLAG_ID);
}

// tnx to kernel
unsigned int _kcpuid_is386()
{
/*  unsigned int cpu_type, before, after;

  __asm__ __volatile__
  ("pushfl               \n"
   "popl %%eax           \n"
   "movl %%eax,%%ecx     \n"
   "xorl $0x40000,%%eax  \n"
   "pushl %%eax          \n"
   "popfl                \n"
   "pushfl               \n"
   "popl %%eax           \n"
   "cmp %%ecx,%%eax      \n"
   "jz is386             \n"
   "pushl %%ecx          \n"
   "popfl                \n"
   "movl $0,%%eax        \n"
   "jmp fine             \n"
   "is386:               \n"
   "movl $1,%%eax        \n"
   "fine:                \n"
   "movl %%eax,%0          "
   : "=m" (cpu_type), "=g"(before), "=g"(after)
  );
  return cpu_type;

  * Replaced with this.
  * If AC is already set, XOR will unset it, so the function will return a false value
  * JZ won't jump to is386 although the original eflags value is different than the current one
  */
  return !chk_eflags(EFLAG_AC);
}

/**
 *   Return a pointer to CPU Vendor ID String
 */
void _kcpuid_0()
{
  static char CPU_VENDOR_STRING[13];
  unsigned long _eax=0, _ebx=0, _ecx=0, _edx=0;

  __asm__ __volatile__
  ("movl $0x00,%%eax \n"  // Select CPUID function
   "cpuid            \n"  // Call CPUID
   "movl %%eax,%0    \n"  // Save EAX (max_cpuid_cnt)
   "movl %%ebx,%1    \n"  // Save EBX (vendor_string)
   "movl %%ecx,%2    \n"  // Save ECX (vendor_string)
   "movl %%edx,%3      "  // Save EDX (vendor_string)
   : "=g" (_eax), "=g" (_ebx), "=g" (_ecx), "=g" (_edx)
 //  :
  // : "ax", "bx", "cx", "dx", "memory"
  );

  // Set max_cpuid_cnt
  CPU.flags.cpuid_max = _eax;
  
  // Extract Vendor String
  CPU_VENDOR_STRING[0] = (char)((_ebx & 0x000000FF));
  CPU_VENDOR_STRING[1] = (char)((_ebx & 0x0000FF00)>>8);
  CPU_VENDOR_STRING[2] = (char)((_ebx & 0x00FF0000)>>16);
  CPU_VENDOR_STRING[3] = (char)((_ebx & 0xFF000000)>>24);
  CPU_VENDOR_STRING[4] = (char)((_edx & 0x000000FF));
  CPU_VENDOR_STRING[5] = (char)((_edx & 0x0000FF00)>>8);
  CPU_VENDOR_STRING[6] = (char)((_edx & 0x00FF0000)>>16);
  CPU_VENDOR_STRING[7] = (char)((_edx & 0xFF000000)>>24);
  CPU_VENDOR_STRING[8] = (char)((_ecx & 0x000000FF));
  CPU_VENDOR_STRING[9] = (char)((_ecx & 0x0000FF00)>>8);
  CPU_VENDOR_STRING[10] = (char)((_ecx & 0x00FF0000)>>16);
  CPU_VENDOR_STRING[11] = (char)((_ecx & 0xFF000000)>>24);
  CPU_VENDOR_STRING[12] = '\0';

  // Return pointer to static string
	CPU.vendor.name = CPU_VENDOR_STRING;
}

/**
 *   Check for CPUID support and print informations
 */
void _kcpuid(void)
{
	memset(&CPU, '\0', sizeof(CPU));
	
  if(_kcpuid_is386()) {
		CPU.flags.is386 = 1;
    return;
  }

	CPU.flags.is386 = 0;
	CPU.flags.cpuid = 0;
  if (_kcpuid_support()) // Check for CPUID support
  {
		CPU.flags.cpuid = 1;

    // Get vendor string and max_cpuid_cnt
    _kcpuid_0();
		
		static _kcpu_names _kcpu_intel_names[] =
		{
			{
				.family = 4, .models =
				{
					[0] = "486 DX-25/33",
					[1] = "486 DX-50",
					[2] = "486 SX",
					[3] = "486 DX/2",
					[4] = "486 SL",
					[5] = "486 SX/2",
					[7] = "486 DX/2-WB",
					[8] = "486 DX/4",
					[9] = "486 DX/4-WB"
				}
			},
			{
				.family = 5, .models =
				{
					[0] = "Pentium 60/66 A-step",
					[1] = "Pentium 60/66",
					[2] = "Pentium 75-200",
					[3] = "Overdrive PODP5V83",
					[4] = "Pentium MMX",
					[7] = "Mobile Pentium 75-200",
					[8] = "Mobile Pentium MMX",
				}
			},
			{
				.family = 6, .models =
				{
					[0] = "Pentium Pro A-step",
					[1] = "Pentium Pro",
					[3] = "Pentium II (Klamath)",
					[4] = "Pentium II (Deschutes)",
					[5] = "Pentium II (Deschutes)",
					[6] = "Mobile Pentium II",
					[7] = "Pentium III (Katmai)",
					[8] = "Pentium III (Coppermine)",
					[10] = "Pentium III (Cascades)",
					[11] = "Pentium III (Tualatin)",
				}
			},
			{
				.family = 15, .models =
				{
					[0] = "Pentium 4 (Unknown)",
					[1] = "Pentium 4 (Willamette)",
					[2] = "Pentium 4 (Northwood)",
					[4] = "Pentium 4 (Foster)",
					[5] = "Pentium 4 (Foster)",
				}
			}
		};
		
		static _kcpu_names _kcpu_amd_names[] =
		{
			{
				.family = 4, .models =
				{
					[3] = "486 DX/2",
					[7] = "486 DX/2-WB",
					[8] = "486 DX/4",
					[9] = "486 DX/4-WB",
					[14] = "Am5x86-WT",
					[15] = "Am5x86-WB",
				}
			},
			{
				.family = 5, .models =
				{
					[0] = "K5/SSA5",
					[1] = "K5",
					[2] = "K5",
					[3] = "K5",
					[6] = "K6",
					[7] = "K6",
					[8] = "K6-2",
					[9] = "K6-3",
				}
			},
			{
				.family = 6, .models =
				{
					[0] = "Athlon",
					[1] = "Athlon",
					[2] = "Athlon",
					[4] = "Athlon",
				}
			},
		};
		
		static _kcpu_names _kcpu_cyrix_names[] =
		{
			{
				.family = 4, .models =
				{
					[4] = "MediaGX",
				}
			},
			{
				.family = 5, .models =
				{
					[2] = "6x86 / 6x86L",
					[4] = "MediaGX MMX Enhanced",
				}
			},
			{
				.family = 6, .models =
				{
					[0] = "m II (6x86MX)",
					[5] = "VIA Cyrix M2 core",
					[6] = "WinChip C5A",
					[7] = "WinChip C5B ,WinChip C5C",
					[8] = "WinChip C5N",
					[9] = "WinChip C5XL, WinChip C5P",
				}
			},
		};
		
		static _kcpu_names _kcpu_umc_names[] =
		{
			{
				.family = 4, .models =
				{
					[1] = "U5D",
					[2] = "U5S",
				}
			},
		};
		
		static _kcpu_names _kcpu_nexgen_names[] =
		{
			{
				.family = 5, .models =
				{
					[0] = "Nx586",
				}
			},
		};
		
		static _kcpu_names _kcpu_rise_names[] =
		{
			{
				.family = 5, .models =
				{
					[0] = "mP6",
					[1] = "mP6",
				}
			},
		};
		
		static _kcpu_names _kcpu_sis_names[] =
		{
			{
				.family = 5, .models =
				{
					[0] = "55x",
				}
			},
		};
		
		static _kcpu_names _kcpu_transmeta_names[] =
		{
			{
				.family = 5, .models =
				{
					[4] = "Crusoe TM3x00 and TM5x00",
				}
			},
		};
		
		static _kcpu_names _kcpu_centaur_names[] =
		{
			{
				.family = 5, .models =
				{
					[4] = "C6",
					[8] = "C2",
					[9] = "C3",
				}
			},
		};
		
		static _kcpu_names _kcpu_nsc_names[] =
		{
			{
				.family = 5, .models =
				{
					[4] = "GX1/GXLV/GXm",
					[5] = "GX2",
				}
			},
		};
		
    // Check CPU vendor string
    if (_kstrncmp(CPU.vendor.name, VENDOR_INTEL_STR, 12) == 0) { // Intel
			CPU.vendor.id = VENDOR_INTEL;
			CPU.name.family = _kcpu_intel_names;
			CPU.name.family_size = 4;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_AMD_STR, 12) == 0) { // AMD
			CPU.vendor.id = VENDOR_AMD;
			CPU.name.family = _kcpu_amd_names;
			CPU.name.family_size = 3;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_CYRIX_STR, 12) == 0) { // Cyrix
			CPU.vendor.id = VENDOR_CYRIX;
			CPU.name.family = _kcpu_cyrix_names;
			CPU.name.family_size = 3;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_UMC_STR, 12) == 0) { // UMC
			CPU.vendor.id = VENDOR_UMC;
			CPU.name.family = _kcpu_umc_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_NEXGEN_STR, 12) == 0) { // NexGen
			CPU.vendor.id = VENDOR_NEXGEN;
			CPU.name.family = _kcpu_nexgen_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_RISE_STR, 12) == 0) { // Rise
			CPU.vendor.id = VENDOR_RISE;
			CPU.name.family = _kcpu_rise_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_SIS_STR, 11) == 0) { // SiS
			CPU.vendor.id = VENDOR_SIS;
			CPU.name.family = _kcpu_sis_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_TRANSMETA_STR, 12) == 0) { // Transmeta
			CPU.vendor.id = VENDOR_TRANSMETA;
			CPU.name.family = _kcpu_transmeta_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_CENTAUR_STR, 12) == 0) { // Centaur
			CPU.vendor.id = VENDOR_CENTAUR;
			CPU.name.family = _kcpu_centaur_names;
			CPU.name.family_size = 1;
		}
		else if (_kstrncmp(CPU.vendor.name, VENDOR_NSC_STR, 12) == 0) { // National Semiconductor
			CPU.vendor.id = VENDOR_NSC;
			CPU.name.family = _kcpu_nsc_names;
			CPU.name.family_size = 1;
		}
		else CPU.vendor.id = VENDOR_UNKNOWN; // Unknown

    // Check if we can call _kcpuid_1()
    if (CPU.flags.cpuid_max < 1) return;
		_kcpuid_1();
		int i;
		if (CPU.vendor.id != VENDOR_UNKNOWN) {
			for (i=0;i<CPU.name.family_size;i++) {
				if (CPU.name.family[i].family == CPU.signature.family)
					CPU.name.cpu_name = CPU.name.family[i].models[CPU.signature.model];
			}
		} else CPU.name.cpu_name = "Processor unknown";
  }
}

/*
 *	Write informations about the CPU.
 *	_kcpuid() must be called to store values.
 */
void _kcpuid_show() {
	if (CPU.flags.is386)
		_kputs("386 found\n");
	else if (!CPU.flags.cpuid)
		_kputs("Your processor doesn't support CPUID\n");
	else {
		_kputs("- Vendor name '"); _kputs(CPU.vendor.name); _kputs("'\n");
		if (CPU.flags.cpuid_max > 0) {
			_kputs("- Processor model '"); _kputs(CPU.name.cpu_name); _kputs("'\n");
			_kputs("- Features:\n");
			if (CPU.features.fpu) _kputs("Floating Point Unit\n");
			if (CPU.features.vme) _kputs("Virtual Mode Extensions\n");
			if (CPU.features.de) _kputs("Debugging Extensions\n");
			if (CPU.features.pse) _kputs("Page Size Extensions\n");
			if (CPU.features.tsc) _kputs("Time Stamp Counter\n");
			if (CPU.features.msr)	_kputs("Model-specific registers\n");
			if (CPU.features.pae) _kputs("Physical Address Extensions\n");
			if (CPU.features.mce)	_kputs("Machine Check Exceptions\n");
			if (CPU.features.cxchg8) _kputs("Compare and exchange 8-byte\n");
			if (CPU.features.apic) _kputs("On-chip APIC\n");
			if (CPU.features.sep) _kputs("Fast System Calls\n");
			if (CPU.features.mtrr) _kputs("Memory Type Range Registers\n");
			if (CPU.features.pge) _kputs("Page Global Enable\n");
			if (CPU.features.mca) _kputs("Machine Check Architecture\n");
			if (CPU.features.cmov) _kputs("Conditional move instruction\n");
			if (CPU.features.pat) _kputs("Page Attribute Table\n");
			if (CPU.features.pse36)	_kputs("36-bit Page Size Extensions\n");
			if (CPU.features.psn) _kputs("Processor Serial Number\n");
			if (CPU.features.clfl) _kputs("CLFLUSH, Description pending\n");
			if (CPU.features.dtes) _kputs("Debug Trace and EMON Store MSRs\n");
			if (CPU.features.acpi) _kputs("Thermal Control MSR\n");
			if (CPU.features.mmx) _kputs("MMX instruction set\n");
			if (CPU.features.fxsr) _kputs("Fast floating point save/restore\n");
			if (CPU.features.sse) _kputs("SSE (Streaming SIMD Extensions)\n");
			if (CPU.features.sse2) _kputs("SSE2 (Streaming SIMD Extensions - #2)\n");
			if (CPU.features.ss) _kputs("Selfsnoop\n");
			if (CPU.features.htt) _kputs("Hyper-Threading Technology\n");
			if (CPU.features.tm1) _kputs("Thermal Interrupt & Status MSRs.\n");
			if (CPU.features.ia64) _kputs("IA-64 (64-bit Intel CPU)\n");
			if (CPU.features.pbe) _kputs("Pending Break Event\n");
		}
	}
}

/*
 *	Get informations from CPUID 1.
 *	It set signature and features about the CPU
 *	without displaying anything.
 */
void _kcpuid_1() {
	asm(
		"mov $0x01, %%eax\n"
		"cpuid\n"
		"mov %%eax, %0\n"
		"mov %%edx, %1\n"
		: "=g"(CPU.signature), "=g"(CPU.features)
	);
}
