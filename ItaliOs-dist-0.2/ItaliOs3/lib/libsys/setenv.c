#include <stdlib.h>

int setenv(const char *name, const char *value, int overwrite){
		asm("int $0x55": :"a"(19), "b"(name),"c"(value));
		return 0;
}
