#ifndef _EXEC_H_
#define _EXEC_H_

bool exec(char* file_name, char* parametri);
void load_dym_lib(char* file_name);
void init_dll_manager();
bool exec_CCC(char* file_name, char* parametri,unsigned char*imagez);

#endif
