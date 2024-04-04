#ifndef __FLOPPY_H
#define __FLOPPY_H

#define FLOPPY_INFOS 0x10
#define START_MOTOR 0x14
#define STOP_MOTOR 0x00

#define FDC_DOR 0x3f2
#define FDC_DRS  (0x3f4)
#define FDC_MSR 0x3f4
#define FDC_DATA 0x3f5

void detect_floppy();
void start_motor();
void stop_motor();
void floppy_irq();
void init_floppy();
void check_version();
int sendbyte(int);
int getbyte();

#endif
