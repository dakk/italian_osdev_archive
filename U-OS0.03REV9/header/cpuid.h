#ifndef __CPUID_H
#define __CPUID_H

#define VENDOR_INTEL_STR  "GenuineIntel"
#define VENDOR_AMD_STR    "AuthenticAMD"

#define VENDOR_UNKNOWN    0
#define VENDOR_INTEL      1
#define VENDOR_AMD        2

int _kcpuid_support(void);
unsigned int _kcpuid_is386();
char * _kcpuid_0(int *);
void _kcpuid(void);
void family_id();

#endif
