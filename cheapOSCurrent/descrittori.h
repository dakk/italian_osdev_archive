
#ifndef	_DESCRIPTOR_
#define	_DESCRIPTOR_

#include	"config.h"

#define	D_LDT	0x200	/*LDT segment		*/
#define	D_TASK	0x500	/*Task gate			*/
#define	D_TSS	0x900	/*TSS			*/
#define	D_CALL	0x0C00	/*386 call gate		*/
#define	D_INT	0x0E00	/*386 interrupt gate	*/
#define	D_TRAP	0x0F00	/*386 trap gate		*/
#define	D_DATA	0x1000	/*Data segment		*/
#define	D_CODE	0x1800	/*Code segment		*/

#define	D_DPL3			0x6000	/*DPL3 o mask DPL	*/
#define	D_DPL2			0x4000	/*DPL2 o mask DPL	*/
#define	D_DPL1			0x2000	/*DPL1 o mask DPL	*/
#define	D_PRESENT		0x8000	/*Present			*/
#define	D_NOT_PRESENT	0x8000	/*Not Present		*/

#define	D_ACC	0x100	/*Accessed(Data o Code)		*/

#define	D_WRITE	0x200	/*Writable(Data segment)	*/
#define	D_READ	0x200	/*Readable (Code segment)	*/
#define	D_BUSY	0x200	/*Busy (TSS)			*/

#define	D_EXDOWN	0x400	/*Cresce in giu'(Data segment)	*/
#define	D_CONFORM	0x400	/*Conforme(Code segment)		*/

#define	D_32BIT		0x40		/*Modo a 32 bit		*/
#define	D_GR4K		0x80		/*Granularita'' a 4k	*/


#define desc_table(name,length) union DT_entry name[length] =

#define stnd_desc(base, limit, control) {desc: {((limit) & 0xffff), ((base) & 0xffff), (((base) >> 16) & 0xff),(((control)+D_PRESENT) >> 8), ((limit)>>16),(((control)&0xff)>>4),((base)>> 24)}}

#define gate_desc(offset, selector, control) {gate: {((offset) & 0xffff), (selector),((control)+D_PRESENT), ((offset) >> 16) }}


struct x86_desc
	{
	unsigned short	limit_low;		/*limit 0..15		*/
	unsigned short	base_low;		/*base  0..15	*/
	unsigned char	base_med;	/*base  16..23	*/
	unsigned char	access;		/*access byte	*/
	unsigned int	limit_high:4;	/*limit 16..19	solo 4 bit!!! */
	unsigned int	granularity:4;	/*granularity	*/
	unsigned char	base_high;	/*base 24..31	*/
	} __attribute__ ((packed));


struct x86_gate
	{
	unsigned short	offset_low;		/*offset 0..15	*/
	unsigned short	selector;		/*selector		*/
	unsigned short	access;		/*access flags	*/
	unsigned short	offset_high;		/*offset 16..31	*/
	} __attribute__ ((packed));

union DT_entry
	{
	struct x86_desc	desc;		/*Descrittore Dati/Stack*/
	struct x86_gate	gate;		/*Descrittore Gate	*/
	unsigned long long dummy;		/*Decrittore Raw!!!	*/
	};


void desc_setstnd(union DT_entry* descrittore,unsigned long base,unsigned long limit,unsigned short control);			//Setta un descrittore standard
void	desc_setgate(union DT_entry* descrittore,unsigned char segment,unsigned long offset,unsigned short control);		//Setta un descrittore gate

unsigned long desc_getstnd_base(union DT_entry* descrittore);		//Estare la base da un descrittore standard
unsigned long desc_getstnd_limit(union DT_entry* descrittore);		//Estare il limit/size da un descrittore standard

#endif
