
#include	"kstring.h"



unsigned int ksprint(char* buffer,const char* stringa,...)
	{
	unsigned int i,j,k,larghezza;

	unsigned long tmpulong;
	char* tmpcharp;
	char	tmpchar;
	char	tmpstrg[16];

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


	return j;
	}

unsigned int kstrlen(const char* stringa)
	{
	unsigned int i;

	for(i=0;stringa[i];i++);

	return i;
	}

char* kstrcpy(char *dest, const char *src)
	{
	unsigned int i;

	for(i=0;src[i];dest[i]=src[i++]);
	dest[i]='\0';

	return dest;
	}

bool kstrcmp(const char* s1,const char* s2)
	{
	int i,n;
	bool opt;

	n=kstrlen(s1);

	if (n!=kstrlen(s2))
		return FALSE;

	for(i=0,opt=TRUE;i<n  && opt;i++)
		opt=(s1[i]==s2[i]);

	return opt;
	}

bool kstrncmp(const char *s1,const char *s2,int  n)
	{
	int i;
	bool opt;
	//Da fare per n negativo!(Ora non ho voglia)
	if (n<0) return -1;

	for(i=0,opt=TRUE;i<n  && opt;i++)
		opt=(s1[i]==s2[i]);

	return opt;
	}

int katoi(char* buffer)
	{
	int num,i;
	bool fine;

	fine=FALSE;
	num=0;

	for(i=0;buffer[i] && !fine;i++)
		{
		num*=10;
		if(buffer[i]<'0' || buffer[i]>'9')  fine=TRUE;
		else
			num+=(buffer[i]-'0');
		}

	return num;
	}

