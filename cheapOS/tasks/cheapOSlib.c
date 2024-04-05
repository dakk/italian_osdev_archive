
#include	"cheapOSlib.h"

extern int main(int argc,char** argv);
int consolefd=-1;

//BASE------------------------------------------

void _start()
	{
	int i;
	int exitval,argc;
	char* argv[TASK_MAXARGNUM];
	char* punt;

	//Prende i parametri in ingresso AGV ARC
	punt=0;
	argc=0;

	asm(
		"nop	\n"
		:"=a"(argc),"=b"(punt)
		:
		);

	for(i=0;i<argc && i<TASK_MAXARGNUM;i++)
		{
		argv[i]=++punt;
		for(;(*punt);punt++);
		}

	argc=i;
	exitval=main(argc,argv);

	//Setta quello di uscita!!
	_exit(exitval);
	}

void _exit(int exitval)
	{
	asm volatile(
		"movl	$0x0100,%%eax	\n"
		"int		$0x41	\n"
		:
		:"c"(exitval)
		);
	}


/*SYSTEM-----------------------------------------*/

bool shutdown()
	{
	bool fatto;

	asm volatile(
		"movl	$0x0000,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(fatto)
		:
		);

	return fatto;
	}


//TASK------------------------------------------

int fork()
	{
	int pid;

	asm volatile(
		"movl	$0x0101,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(pid)
		:
		);

	return pid;
	}

int getpid()
	{
	int pid;

	asm volatile(
		"movl	$0x0102,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(pid)
		:
		);

	return pid;
	}

unsigned long wait(unsigned long ticks)
	{
	int ticks_effettivi;

	asm volatile(
		"movl	$0x0103,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(ticks_effettivi)
		:"c"(ticks)
		);

	return ticks_effettivi;
	}

int waitpid(int pid)
	{
	int exitcode;

	asm volatile(
		"movl	$0x0104,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(exitcode)
		:"c"(pid)
		);

	return exitcode;
	}

int exec(char* nomefile,int argc,char* argv0)
	{
	int pid;

	asm volatile(
		"movl	$0x0105,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(pid)
		:"c"(nomefile),"b"(argc),"d"(argv0)
		);

	return pid;
	}

bool kill(int pid)
	{
	bool killed;

	asm volatile(
		"movl	$0x0106,%%eax	\n"
		"int		$0x41	\n"
		:"=b"(killed)
		:"c"(pid)
		);

	return killed;
	}

void exit(int exitcode)
	{
	_exit(exitcode);
	//Qui non arriva mai!!
	return;
	}
	
//FILESYSTEM--------------------------------------------------------------

int open(char* nomefile)
	{
	int fd;

	asm volatile(
		"movl	$0x0200,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(fd)
		:"c"(nomefile)
		);

	return fd;
	}

bool close(int fd)
	{
	bool opt;

	asm volatile(
		"movl	$0x0201,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(opt)
		:"c"(fd)
		);

	return opt;
	}

unsigned int seek(int fd,int modo,int posizione)
	{
	unsigned int posizioneora;

	asm volatile(
		"movl	$0x0202,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(posizioneora)
		:"c"(fd),"b"(posizione),"d"(modo)
		);

	return posizioneora;
	}

int read(int fd,int lunghezza,char* buffer)
	{
	int opt;

	asm volatile(
		"movl	$0x0203,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(opt)
		:"c"(fd),"b"(lunghezza),"d"(buffer)
		);

	return opt;
	}

int write(int fd,int lunghezza,char* buffer)
	{
	int opt;

	asm volatile(
		"movl	$0x0204,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(opt)
		:"c"(fd),"b"(lunghezza),"d"(buffer)
		);

	return opt;
	}

bool touch(char*nomefile)
	{
	bool opt;

	asm volatile(
		"movl	$0x0205,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(opt)
		:"c"(nomefile)
		);

	return opt;
	}

bool remv(char* nomefile)
	{
	bool opt;

	asm volatile(
		"movl	$0x0206,%%eax	\n"
		"int		$0x41			\n"
		:"=b"(opt)
		:"c"(nomefile)
		);

	return opt;
	}

//UTILI---------------------------------------------------------------------------
bool openconsole(int consolenum)
	{
	char buffer[24];

	if(consolenum>4 || consolenum<1)
		return FALSE;
	if(consolefd>=0)
		return FALSE;

	sprint(buffer,"CON:\\Console%d",consolenum);
	consolefd=open(buffer);

	return !(consolefd<0);
	}

bool closeconsole()
	{
	if(consolefd<0)
		return FALSE;
	if(close(consolefd))
		{
		consolefd=-1;
		return TRUE;
		}
	return FALSE;
	}

int	print(const char* stringa,...)
	{
	unsigned int i,j,k,larghezza;

	unsigned long tmpulong;
	char* tmpcharp;
	char	  tmpchar;

	char	tmpstrg[NUM_LUNG];
	char buffer[STRNG_LUNG];

	char* argomenti; // puntatore agli argomenti

	argomenti=(char *) &stringa + sizeof(stringa);

	for(i=j=0;stringa[i] && j<STRNG_LUNG;i++)
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


					for(;k>0 && j<STRNG_LUNG;k--)
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

					for(;k>0 && j<STRNG_LUNG;k--)
						buffer[j++]=tmpstrg[(k-1)];

					argomenti+=sizeof(unsigned long);
				break;

				case 's':

					k=0;
					tmpcharp=*((char**)argomenti);

					for(k=0;tmpcharp[k] && j<STRNG_LUNG;buffer[j++]=tmpcharp[k++]);
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

 	baseprint(buffer);
	return 0;
	}


int baseprint(char* stringa)
	{
	int i,opt;

	if(consolefd<0)
		return -1;

	for(i=0,opt=1;i<strlen(stringa) && opt>0;i++)
		opt=write(consolefd,1,&(stringa[i]));
	return i;
	}


unsigned char	getchar()
	{
	unsigned char carattere;
	if(consolefd<0)
		return -1;
	read(consolefd,1,&carattere);

	return carattere;
	}


int	getstring(char* buffer,int maxcars)
	{
	unsigned char c;
	int cars,i;

	cars=0;

	do
		{
		switch(c=getchar())
			{
			case '\n':
				buffer[cars++]=c;
			break;

			case '\b':
				if(cars>0)
					{
					cars--;
					print("%c",c);
					}
			break;

			case CHOME:
				//Non Accade Nulla!
			break;

			case CCLEAR:
				for(i=cars;i>0;i--)
					print("\b");
				cars=0;
			break;

			case '\t':
			case CSX:
			case CDX:
			case CUP:
			case CDOWN:
				//Non Accade Nulla!
			break;

			case CBLACK:
			case CDKBLUE:
			case CDKGREEN:
			case CDKCYAN:
			case CDKRED:
			case CDKPURPLE:
			case CDKBROWN:
			case CLTGRAY:
			case CDKGRAY:
			case CLTBLUE:
			case CLTGREEN:
			case CLTCYAN:
			case CLTRED:
			case CLTPURPLE:
			case	CLTBROWN:
			case	CWHITE:
			case	CREVERSE:
			case	CBLINK:
				//Stampa il carattere ma non lo bufferizza!
				print("%c",c);
			break;

			default:
				print("%c",c);
				buffer[cars++]=c;
			}
		}while(c!='\n' && cars<maxcars);


	buffer[cars++]='\0';

	return cars;
	}



int	random()
	{
	char buffer[8];
	bool opt;
	int fd,tento,aux;
	int ran;

	fd=-1;
	tento=128;

	while(fd<0 && tento>0)
		{
		fd=open("KER:\\random");
		tento--;
		}

	if (tento<=0)
		return -1;

	tento=read(fd,4,buffer);
	buffer[tento]='\0';

	for(ran=0,tento--;tento>=0;tento--)
		{
		aux=buffer[tento]<<tento;
		ran+=aux;
		}

	opt=TRUE;
	tento=128;
	while(opt && tento>0)
		{
		opt=!close(fd);;
		tento--;
		}

	if (tento<=0)
		return -1;


	return ran;
	}

	
int	randomInt(int min,int max)
	{
	int j;

	j=min+(random()%(max-min+1));

	return j;
	}

unsigned int strlen(const char* stringa)
	{
	unsigned int i;

	for(i=0;stringa[i];i++);

	return i;
	}

char* strcpy(char *dest, const char *src)
	{
	unsigned int i;

	for(i=0;src[i];dest[i]=src[i++]);
	dest[i]='\0';

	return dest;
	}

bool strcmp(const char* s1,const char* s2)
	{
	int i,n;
	bool opt;

	n=strlen(s1);

	if (n!=strlen(s2))
		return FALSE;

	for(i=0,opt=TRUE;i<n  && opt;i++)
		opt=(s1[i]==s2[i]);

	return opt;
	}

bool strncmp(const char *s1,const char *s2,int  n)
	{
	int i;
	bool opt;
	//Da fare per n negativo!(Ora non ho voglia)
	if (n<0) return -1;

	for(i=0,opt=TRUE;i<n  && opt;i++)
		opt=(s1[i]==s2[i]);

	return opt;
	}

unsigned int sprint(char* buffer,const char* stringa,...)
	{
	unsigned int i,j,k,larghezza;

	unsigned long tmpulong;
	char* tmpcharp;
	char	tmpchar;
	char	tmpstrg[16];

	char* argomenti; // puntatore agli argomenti

	argomenti=(char *) &stringa + sizeof(stringa);

	for(i=j=0;stringa[i] && j<STRNG_LUNG;i++)
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


					for(;k>0 && j<STRNG_LUNG;k--)
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

					for(;k>0 && j<STRNG_LUNG;k--)
						buffer[j++]=tmpstrg[(k-1)];

					argomenti+=sizeof(unsigned long);
				break;

				case 's':

					k=0;
					tmpcharp=*((char**)argomenti);

					for(k=0;tmpcharp[k] && j<STRNG_LUNG;buffer[j++]=tmpcharp[k++]);
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

	return j;
	}

int atoi(char* buffer)
	{
	int num,i;
	bool fine;

	fine=FALSE;
	num=0;

	for(i=0;buffer[i] && !fine;i++)
		{
		if(buffer[i]<'0' || buffer[i]>'9')  fine=TRUE;
		else
			{
			num*=10;
			num+=(buffer[i]-'0');
			}
		}

	return num;
	}

