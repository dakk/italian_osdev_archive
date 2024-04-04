
#include	"cheapOSlib.h"

#define MAXTENTATIVI	20

void tab(int n);
void tanti(char c,int n);

int main(int argc,char*argv[])
	{
	int i,j,lung;
	int console,tentativi;
	char buffer[12];
	char segreto[12];
	int op1,op2;
	int loob[5];
	int loob1[5];

	console=1;

	if(argc==2)
		console=atoi(argv[1]);

	if(!openconsole(console))
		exit(1);

	for(i=0;i<5;i++)
		{
/*		do
			{
*/			segreto[i]=randomInt(0,9)+'0';
/*
			ok=TRUE;
			for(j=0;j<i;j++)
				if(segreto[i]==segreto[j])
					ok=FALSE;
			}
		while(!ok);
*/		}

	segreto[i]='\0';

	print("%c",CCLEAR);
	print("%c",CHOME);
	print("MasterMind!\n\tTentativi\tNumero\tNumeroGiusto\tPostoGiusto\n");

	for(tentativi=MAXTENTATIVI;tentativi>0;tentativi--)
		{
		tab(6);print("%2d",tentativi);tab(7);
riprova:
		lung=getstring(buffer,5)-1;

		if(buffer[0]=='\n')
			goto riprova;

		if(strcmp(buffer,"quit")) goto fine;

		if(lung!=5)
			{
			for(i=0;i<lung;i++)
				print("\b");
			goto riprova;
			}

		for(i=0;i<lung;i++)
			print("\b");

		for(i=0;i<lung;i++)
			{
			if(buffer[i]<'0' || buffer[i]>'9')
				goto riprova;
			else
				{
				print("%c",180+(buffer[i]-'0'));
				print("%c",buffer[i]);
				}
			}

		op2=op1=0;
		for(i=0;i<5;i++)
			{
			loob[i]=0;
			loob1[i]=0;
			}
		for(i=0;i<5;i++)
			{
			if(buffer[i]==segreto[i])
				{
				op2++;
				loob[i]=1;
				loob1[i]=1;
				}
			}
		for(i=0;i<5;i++)
			{
			for(j=0;j<lung;j++)
				if(buffer[i]==segreto[j] && loob[j]==0 && loob1[i]==0)
					{
					op1++;
					loob[j]=1;
					loob1[i]=1;
					}
			}

		print("%c",CWHITE);
		tab(7);tanti(2,op1);tab(13-op1);tanti(1,op2);
		print("\n");

		if(strcmp(segreto,buffer))
			goto vinto;
		}

	goto fine;

vinto:
	print("\nBravo! Hai indovinato in %d tentativi",MAXTENTATIVI-tentativi+1);

fine:
	print("\nIl numero era:");

	for(i=0;i<5;i++)
			{
			print("%c",180+(segreto[i]-'0'));
			print("%c",segreto[i]);
			}
	print("%c\n",CWHITE);

	closeconsole();

	return 0;
	}


void tab(int n)
	{
	int i;

	for(i=0;i<n;i++)
		print(" ");

	return;
	}

void tanti(char c,int n)
	{
	int i;

	for(i=0;i<n;i++)
		print("%c",c);

	return;
	}
