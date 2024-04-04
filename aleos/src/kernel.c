/* AleOS kernel source                  *
* Copyright (c) 2005 Alessandro Presta */

#include <kernel.h>
#include <app.h>

void _start()
{
	main();
	while (1);
}
		
int main()
{
	app();
	return 0;
}
