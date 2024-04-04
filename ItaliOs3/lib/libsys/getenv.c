#include <stdlib.h>

char *getenv(const char *name){
		char* value = (char*) malloc(1000);
		asm("int $0x55": :"a"(18), "b"(name),"c"(value));
		return value;
}
