
#include "kconsole.h"


unsigned char*	kVideoMem;//Variabili per la printk interna!
unsigned	int		kCursore;


void kconsole_init()
	{
	kVideoMem=(unsigned char*)0xB8000; // Puntatore alla memoria video(uso interno)
	kCursore=0;//Forzo un clear all'inizio!
	kclrscr();
	}

void kclrscr()
	{
	unsigned int i;

	for(i=0;i<(KONS_X*KONS_Y*2);i+=2)
		{
		kVideoMem[i]=' ';
		kVideoMem[i+1]=0x07;
		}
	}

void kscrllscr()
	{
	unsigned int i;

	for(i=0;i<(KONS_X*(KONS_Y-1)*2);i+=2)
		{
		kVideoMem[i]=kVideoMem[i+(KONS_X*2)];
		kVideoMem[i+1]=kVideoMem[i+1+(KONS_X*2)];
		}
	for(;i<(KONS_X*KONS_Y*2);i+=2)
		{
		kVideoMem[i]=' ';
		kVideoMem[i+1]=0x07;
		}
	}

void kbaseprint(const char*stringa)
	{
	unsigned int i;

	for (i=0;stringa[i];i++)
		{

		if(kCursore>(KONS_Y*KONS_X*2)-1)//Arrivato infondo cancella lo schermo e comincia daccapo!
			{
			kscrllscr();
			kCursore%=(KONS_Y*KONS_X*2);
			kCursore+=(KONS_X*(KONS_Y-1)*2);
			}

		switch(stringa[i])
			{
			case '\n' :	//Accapo
				kCursore=((kCursore/(KONS_X*2))+1)*(KONS_X*2);
			break;

			case '\t' :	//Tabulazione
				kCursore+=(TAB_LUNG*2);
			break;

			default : //Carattere standard
				kVideoMem[kCursore++]=stringa[i];
				kVideoMem[kCursore++]=0x07;	//Bianco su nero!

			}
		}

	return;
	}

void kprint(const char* stringa,...)
	{
	unsigned int i,j,k,larghezza;

	unsigned long tmpulong;
	char* tmpcharp;
	char	  tmpchar;

	char	tmpstrg[KNUM_LUNG];
	char buffer[KSTRNG_LUNG];

	char* argomenti; // puntatore agli argomenti

	argomenti=(char *) &stringa + sizeof(stringa);

	//argomenti+=KSTACK_BASE;

	for(i=j=0;stringa[i] && j<KSTRNG_LUNG;i++)
		{
		if(stringa[i]=='%')
			{

			if(stringa[i+1]>='0' && stringa[i+1]<='9')
				larghezza=stringa[++i]-'0';
			else
				larghezza=0;


			switch(stringa[++i])
				{
				case 'd':
					k=0;
					tmpulong=*((unsigned long*)argomenti);

						do
							{
							tmpstrg[k]=tmpulong%10;
							tmpulong/=10;

							tmpstrg[k++]+='0';
							}
						while (tmpulong>=1);

					for(;k<larghezza;tmpstrg[k++]='0');//ajjunge Zeri!


					for(;k>0 && j<KSTRNG_LUNG;k--)
						buffer[j++]=tmpstrg[(k-1)];

					argomenti+=sizeof(unsigned long);

				break;

				case 'x':

					k=0;
					tmpulong=*((unsigned long*)argomenti);

					do
						{
						tmpstrg[k]=tmpulong%16;
						tmpulong/=16;

						tmpstrg[k]+=((tmpstrg[k]<10)?'0':('A'-10));
						k++;
						}
					while (tmpulong>=1);

					for(;k<larghezza;tmpstrg[k++]='0');//ajjunge Zeri!

					for(;k>0 && j<KSTRNG_LUNG;k--)
						buffer[j++]=tmpstrg[(k-1)];

					argomenti+=sizeof(unsigned long);
				break;

				case 's':

					k=0;
					tmpcharp=*((char**)argomenti);

					for(k=0;tmpcharp[k] && j<KSTRNG_LUNG;buffer[j++]=tmpcharp[k++]);
					//j--;

					argomenti+=sizeof(char*);

				break;

				case 'c':	//stampa un carattere

					tmpchar=*((char*)argomenti);

					buffer[j++]=tmpchar;

					argomenti+=sizeof(char);

				break;

				default:
					buffer[j++]='?';
				}

			}
		else
			buffer[j++]=stringa[i];
		}

	buffer[j]='\0';

	kbaseprint(buffer);

	return;
	}

/*
void kprint(char* stringa,...)//uso interno!
	{
	unsigned int i;
	char* stringaespansa;

	for(i=0;stringa[i] && stringa[i]!='%';i++);

	if(!stringa[i])
		kprint(stringa);
	else
		{
		stringaespansa=(char*))mem_alloc(KSTRING_LUNG*sizeof(char))

		asm volatile asm("push %0"::"a"(string));
		for(i=0;stringa[i];i++)

		//stringaespansa=ksprint()//Come si passano gli argomenti??(usare asm??Un Po' Brutto!)

		kprint(stringaespansa);

		mem_free(stringaespansa);//Libera la memoria!!
		}

	}


*/


