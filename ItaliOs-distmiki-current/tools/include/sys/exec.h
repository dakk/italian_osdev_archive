#include <syscall.h>
#ifndef _EXEC_H_
#define _EXEC_H_

inline int exec(char* file_name, char* par){
	    int ret;
	    asm volatile ("int $0x55\n":"=a"(ret): "a"(17), "b"(file_name), "c"(par));
	     return ret;
	}

inline int exec_CCC(char* file_name, char* par,unsigned char *immagine){
	    int ret;
	    asm volatile ("int $0x55\n":"=a"(ret): "a"(SYS_EXEC_CCC), "b"(file_name), "c"(par), "d"(immagine));
            return ret;
	}

#endif
