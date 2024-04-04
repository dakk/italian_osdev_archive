#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdio.h>
#define assert(condizione) if( ! (condizione )){printf("ASSERT: FILE=%s, LINE=%d\n", __FILE__, __LINE__ ); asm("cli"); for(;;);}   

#endif
