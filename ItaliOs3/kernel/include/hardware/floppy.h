#ifndef _FLOPPY_H_
#define _FLOPPY_H_

void init_floppy();
bool read_block(int block, byte * blockbuff, size_t nosectors);
bool write_block(int block, byte * blockbuff, unsigned long nosectors);
int floppy_r_buff(int puntat,byte *buff,int size);
int floppy_w_buff(int puntat,byte *buff,int size);

#endif
