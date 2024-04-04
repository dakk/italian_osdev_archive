/* AleOS math source                    *
* Copyright (c) 2005 Alessandro Presta */

#include <math.h>

double pow(double base, int exp)
{
	double result;
	int i;
	result = 1;
	for (i = 0; i < exp; i++)
		result *= base;
	return result;
}
