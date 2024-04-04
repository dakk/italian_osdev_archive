/* AleOS system header                  *
 * Copyright (c) 2005 Alessandro Presta */

/* halt function */
#define halt() __asm__ __volatile__ ( "hlt" )
