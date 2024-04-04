#include <cpuid.h>
#include <video.h>
#include <string.h>
#include <stdio.h>

int _kcpuid_support(void)
{
  int a;

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
}

unsigned int _kcpuid_is386()
{
  unsigned int cpu_type;

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
   : "=m" (cpu_type)
  );

  return cpu_type;
}

char * _kcpuid_0(int *max_cpuid_cnt)
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
   :
   : "ax", "bx", "cx", "dx", "memory"
  );

  *max_cpuid_cnt = _eax;
  
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

  return CPU_VENDOR_STRING;
}

void _kcpuid(void)
{
  if(_kcpuid_is386()){
    _kputs("CPUID: 386 found\n");
    return;
  }

  if (_kcpuid_support())
  {
    int max_cpuid_cnt=0, vendor;
    char *vendor_string;

    _kputs("CPUID: support found!\n");

    vendor_string = _kcpuid_0(&max_cpuid_cnt);
    
    if (_kstrncmp(vendor_string, VENDOR_INTEL_STR, 12) == 0) // Intel
      vendor = VENDOR_INTEL;
    else if (_kstrncmp(vendor_string, VENDOR_AMD_STR, 12) == 0) // AMD
      vendor = VENDOR_AMD;
    else // Unknown
      vendor = VENDOR_UNKNOWN;

    if (!vendor) {
      _kputs("CPUID: Vendor ID String UNKNOWN '"); _kputs(vendor_string); _kputs("'\n");
    } else {
      _kputs("CPUID: Vendor ID String OK '"); _kputs(vendor_string); _kputs("'\n");
    }

    if (max_cpuid_cnt < 1) {
      _kputs("CPUID: max_cpuid_cnt<1, bailing out!\n");
      goto CPUID_DONE;
    }
  }
  else _kputs("CPUID: Your processor doesn't support CPUID\n");

CPUID_DONE:
  
  _kputs("CPUID: Done.\n");
}

void family_id() {
int family;
asm( "movl $0x01, %%eax\n"
       "cpuid\n"
       "movl %%eax,%0    \n" : "=a"(family));
printf("\n%d\n", family);
}
