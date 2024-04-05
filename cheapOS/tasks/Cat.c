
#include	"cheapOSlib.h"

#define	MAXTOTALI	1600
#define	RIGA		80

char buffer[60];

int main(int argc,char** argv)
	{
	int fd;
	int caratteri,prove,cartotali,inizio;
	bool opt;
	int console;
	unsigned char c;
	char nomefile[26];

	console=1;

	if(argc==2)
		console=atoi(argv[1]);

	if(!openconsole(console))
		exit(-1);

	if(argc!=2)
		{
		print("Uso:\n%s <console>\n",argv[0]);
		return -1;
		}

	//print("%c%c",CCLEAR,CHOME);
	print("Cat\nFile:");getstring(nomefile,24);print("\n");
	print("%s\n",nomefile);
	fd=-1;
	prove=3;
	while(fd<0 && prove>0)
		{
		fd=open(nomefile);
		prove--;
		wait(100);
		}

	if (fd<0)
		{
		print("Apertura Fallita\n");
		return -1;
		}

	inizio=seek(fd,MREL,0);
	c=' ';
	caratteri=1;
	while(c!='q')
		{

		cartotali=0;
		seek(fd,MINIZIO,inizio);
		while(caratteri>0 && cartotali<MAXTOTALI)
			{
			caratteri=read(fd,60,buffer);
			buffer[caratteri]='\0';
			if(caratteri>0)
				{
				print(buffer);
				cartotali+=caratteri;
				}
			}

		c=getchar();

		switch(c)
			{
			case CUP:
				if(inizio>0)
					inizio--;
			break;

			case CDOWN:
					inizio++;
			break;

			default:
				c=c;/*Nulla!*/
			}
		}

	opt=FALSE;
	prove=3;
	print("\n");

	while(!opt && prove>0)
		{
		opt=close(fd);
		prove--;
		}

	if(!opt)
		{
		print("\nChiusura Fallita\n");
		return -1;
		}

	closeconsole();

	return 0;
	}


