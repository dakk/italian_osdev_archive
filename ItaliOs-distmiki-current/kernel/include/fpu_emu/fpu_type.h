#ifdef __FPU_EMU__

#ifndef __FPU_TYPE__
#define __FPU_TYPE__

#include <cstddef>

namespace myfloat {

struct mdbl{
        unsigned long long number;
	unsigned int exp:15;
	unsigned int sign:1;
	unsigned int inutile:16;	
};

union my_double {
	struct mdbl strutturato;
	long double numero;
};

struct mflt{
	unsigned int number:23;
	unsigned int exp:8;
	unsigned int sign:1;
};

union my_float {
	struct mflt strutturato;
	float numero;
};

//double
struct msdb{
	unsigned long long number:52;
	unsigned int exp:11;
	unsigned int sign:1;
};

union my_sdouble {
	struct msdb strutturato;
	double numero;
};

}

#endif //__FPU_TYPE__

#endif //__FPU_EMU__
