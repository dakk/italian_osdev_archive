
#include	"io.h"

inline unsigned char inportb(unsigned int port)			/*Legge un byte*/
	{
	unsigned char ret;

	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
	}

inline unsigned int inportw(unsigned int port)				/*Legge una word*/
	{
	unsigned int ret;

	asm volatile ("inw %%dx,%%ax":"=a" (ret):"d" (port));
	return ret;
	}

inline void outportb(unsigned int port,unsigned char value)	/*Manda un byte*/
	{
	asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));
	}

inline void outportw(unsigned int port,unsigned int value)		/*Manda una  word*/
	{
	asm volatile ("outw %%ax,%%dx"::"d" (port), "a" (value));
	}
