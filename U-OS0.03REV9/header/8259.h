#ifndef __8259_H
#define __8259_H

#include <mydef.h>

#define MASTER_PIC 0x20
#define SLAVE_PIC 0xA0
 
#define MASTER2_PIC 0x21
#define SLAVE2_PIC 0xA1

#define ICW1 0x11
 
#define ICW2_M 0x20
#define ICW2_S 0x28

#define ICW3_M 0x04
#define ICW3_S 0x03

#define ICW4_M 0x01
#define ICW4_S 0x01

#define READ_ISR 0x0B
#define EOI 0x20

void initPIC8259();
void enable_irq(byte, byte);
void disable_irq(byte, byte);
//void exec_irq();

#endif
