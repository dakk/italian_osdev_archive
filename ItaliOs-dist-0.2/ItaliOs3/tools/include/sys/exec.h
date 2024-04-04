#ifndef _EXEC_H_
#define _EXEC_H_

inline int exec(char* file_name, char* par){
	    int ret;
	    asm volatile ("int $0x55\n":"=a"(ret): "a"(17), "b"(file_name), "c"(par));
	     return ret;
	}

#endif
