#include <floppy.h>
#include <stdio.h>
#include <idt.h>
#include <irq.h>
#include <8259.h>
#include <io.h>

int motor;
char* floppy_strings[6] = {"No Floppy-Drive","360kb - 5,25","1,2mb - 5,25","720kb - 3,5","1,44mb - 3.5", "2,88mb - 3,5"};

void init_floppy() {

    outportb(0x00, FDC_DOR);
    motor= FALSE;
    outportb(0x0c, FDC_DOR);
    enable_irq(0xBC, MASTER2_PIC);
    add_irq(floppy_irq, 6);
    check_version();
}

void check_version() {

        int info;
        sendbyte(FLOPPY_INFOS);
        info = getbyte();
        if(info == 0x80)
            printf("    \t Found controller nec765....\n\n");
        else if(info==0x90)
            printf("    \t Found controller extended floppy...\n\n");
        else
            printf("    \t Disowned Controller...\n\n");
}
void detect_floppy()
{

    unsigned char floppy_info;
    outportb(FLOPPY_INFOS, 0x70);
    floppy_info = inportb(0x71);
    printf("\n    \t Floppy 1: %s", floppy_strings[floppy_info>>4]);
    printf("\n    \t Floppy 2: %s\n", floppy_strings[floppy_info&0x0F]);
}

void floppy_irq() {
}

void start_motor()
{
    if(motor==FALSE) {
        outportb(0x1C,FDC_DOR);
        motor=TRUE;
    }
}

void stop_motor()
{
    if(motor==TRUE) {
        outportb(0x0c,FDC_DOR);
        motor=FALSE;
    }
}

int sendbyte(int databyte) {
    int timeout;
    int msrbyte;
    for(timeout=0; timeout<128; timeout++) {
        msrbyte = inportb(FDC_MSR);
        if((msrbyte & 0xc0) == 0x80) {
            outportb(databyte, FDC_DATA);
            return 0;
        }
    }
    inportb(0x80);
    return -1;
}

int getbyte(){
    int timeout;
    int msrb;
    for(timeout=0; timeout<128; timeout++) {
        msrb=inportb(FDC_MSR);
        if((msrb & 0xc0) == 0xc0)
            return inportb(FDC_DATA);
        inportb(0x80);
    }
    return -1;
}

int calibrate() {
    outportb(0x1c, FDC_DOR);
}
