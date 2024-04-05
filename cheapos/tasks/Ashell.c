
#include	"cheapOSlib.h"

#define	CMD_NUM	13

char	docat_buffer[60];
char	commandline[60];
char	buffer[60];

char path[STRNG_LUNG]="FAT:\\";

char	comando[CMD_NUM][12]=
	{
	"cd",
	"cd..",
	"ls",
	"vol",
	"cat",
	"exec",
	"execbg",
	"kill",
	"killall",
	"shutdown",
	"exit",
	"help",
	"cls",
	};

int 	tokenizer(char* buffer,char*nextoken,int cur);

bool	docat(char*nomefile);
int	doexec(char* nomefile,char* argomenti);
void	dohelp(char* argomento);

int main(int argc,char*argv[])
	{
	int i,cur,lung,cmd,pid;
	bool running;
	int console;
	char c;
	char token[24];

	/*Apre una console,la 1 di default*/
	console=1;
	if(argc==2)
		{
		console=atoi(argv[1]);
		if(console<1 || console>4)
			console=1;
		}
	if(!openconsole(console))
		return 0;
	print("Ashell on Console %d\n",console);

	running=TRUE;
	while(running)
		{
		do
			{
			print("%s>",path);
			print("%c",CDKGREEN);
			lung=getstring(commandline,60);
			print("%c\n",CWHITE);
			cur=tokenizer(commandline,token,0);
			}
		while(cur>lung);

		cmd=-1;
		for(i=0;i<CMD_NUM;i++)
			if(strcmp(token,comando[i])) cmd=i;

		switch(cmd)
			{
			case	0: /*cd */
				cur=tokenizer(commandline,token,cur);

				if (cur>lung) break;

				sprint(buffer,"%s%s\\",path,token);
				i=open(buffer);
				if(i>=0)
					{
					close(i);
					sprint(path,"%s%s\\",path,token);
					}
				else
					print("Errore\n");
			break;

			case	1: /*cd..*/
				if(strlen(path)>4)
						{
						for(i=strlen(path)-2;path[i]!='\\';i--);
						path[++i]='\0';
						}
				else
					print("Errore\n");
			break;

			case	2:	/*ls*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					sprint(buffer,"%s",path);
				else
					sprint(buffer,"%s%s\\",path,token);

				if(!docat(buffer))
					print("Errore\n");
			break;

			case 	3: /*vol*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					{
					dohelp("vol ");
					break;
					}

				if(strlen(token)==4 && token[3]==':')
					{
					sprint(path,"%s\\",token);
					}
				else
					dohelp("vol ");
			break;

			case	4:	/*cat*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					{
					dohelp("cat ");
					break;
					}

				sprint(buffer,"%s%s",path,token);

				if(!docat(buffer))
					print("Errore\n");
			break;

			case	5:/*exec*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					{
					dohelp("exec ");
					break;
					}

				sprint(buffer,"%s%s",path,token);

				closeconsole();

				pid=doexec(buffer,&(commandline[cur]));

				if(pid>=0)
					waitpid(pid);
				else
					print("Errore\n");

				if(!openconsole(console))
					exit(-1);
			break;

			case	6:/*execbg*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					{
					dohelp("execbg");
					break;
					}

				sprint(buffer,"%s%s",path,token);

				pid=doexec(buffer,&(commandline[cur]));

				if(pid>0)
					print("\"%s\" eseguito con Pid %3d\n",buffer,pid);
				else
					print("Errore\n");
			break;

			case 7: /*kill*/
				cur=tokenizer(commandline,token,cur);

				if (cur>lung)
					{
					dohelp("kill ");
					break;
					}

				if(strlen(token)>3 || token[0]<'0' || token[0]>'9')
					{
					dohelp("kill ");
					break;
					}

				pid=atoi(token);
				do
					{
					print("Kill Task %3d?(S/N)",pid);
					c=getchar();
					print("\n");
					}
				while(c!='s' && c!='n');

				if(c=='s')
					{
					if(kill(pid))
						print("Task %d Killed\n",pid);
					else
						print("Errore\n");
					}
			break;

			case	8:	/*killall*/
				pid=getpid();
				cur=0;
				do
					{
					print("Kill All Tasks?(S/N)",pid);
					c=getchar();
					print("\n");
					}
				while(c!='s' && c!='n');

				for(i=2;i<MAX_TASK_NUM;i++)
					if(i!=pid)
						if(kill(i))
							cur++;
				print("Killed %d tasks\n",cur);
			break;


			case	9:	/*shutdown*/
				do
					{
					print("Shutdown?(S/N)");
					c=getchar();
					print("\n");
					}
				while(c!='s' && c!='n');

				if(c=='n') break;

				do
					{
					print("Davvero?(S/N)");
					c=getchar();
					print("\n");
					}
				while(c!='s' && c!='n');

				if(c=='n') break;

				if(!shutdown())
					print("Errore\n");
				else
					print("OK\n");
			break;

			case	10:	/*exit*/
				running=FALSE;
			break;

			case	11: /*help*/
				dohelp(&(commandline[cur]));
			break;

			case	12: /*help*/
				print("%c",CCLEAR);
				print("%c",CHOME);
			break;

			default:
				print("Comando Sconosciuto\n");
			}
		}

	closeconsole(console);

	return 0;
	}


int tokenizer(char* buffer,char*nextoken,int cur)
	{
	int i;
	bool vai,token;

	i=0;vai=TRUE;token=FALSE;

	while(vai)
		{
		switch(buffer[cur])
			{
			/*Caratteri di Separazione*/
			case	' ':
			case 	'\n':
			case	'\0':
			if(token) vai=FALSE;
			break;

			default:
				if (!token) token=TRUE;
				nextoken[i++]=buffer[cur];
			}
		cur++;
		}

	nextoken[i]='\0';

	return cur;
	}


int	doexec(char* nomefile,char* argomenti)
	{
	int pid,cur,opt,lung;
	int argc_param;
	char token[24];
	char argv0_param[60];

	/*Il primo argomento e' il nome con cui il file e' chiamato!*/
	argc_param=1;
	opt=sprint(&(argv0_param[0]),"%s",nomefile);

	/*Altri argomenti*/
	cur=0;
	lung=strlen(argomenti);

	while(cur<lung)
		{
		cur=tokenizer(argomenti,token,cur);
		opt=sprint(&(argv0_param[opt+1]),"%s",token);
		argc_param++;
		}

	/*Apro il file*/
	pid=exec(nomefile,argc_param,argv0_param);

	return pid;
	}


bool docat(char*nomefile)
	{
	int fd;
	int caratteri,prove;
	bool opt;

	fd=-1;
	prove=2;
	while(fd<0 && prove>0)
		{
		fd=open(nomefile);
		prove--;
		wait(100);
		}

 	if (fd<0)
		{
		print("Apertura Fallita\n");
		return FALSE;
		}

	caratteri=1;
	while(caratteri>0)
		{
		caratteri=read(fd,60,docat_buffer);
		docat_buffer[caratteri]='\0';
		if(caratteri>0)print(docat_buffer);
		}

	prove=2;
	print("\n");

	opt=FALSE;
	while(!opt && prove>0)
		{
		opt=close(fd);
		prove--;
		}

	if(!opt)
		{
		print("\nChiusura Fallita\n");
		return FALSE;
		}

	return TRUE;
	}

void	dohelp(char* argomento)
	{
	int i,cmd,lung,cur;
	char token[24];

	lung=strlen(argomento);
	cur=tokenizer(argomento,token,0);

	cmd=11;
	if(cur<=lung)
		{
		for(i=0;i<CMD_NUM;i++)
			if(strcmp(token,comando[i])) cmd=i;
		}

	print("Help on %s:\n",comando[cmd]);
	switch(cmd)
		{
		case 0:
			print("Uso: cd <directory>\n");
			print("Scende nella directory (cammino relativo).\n");
		break;

		case 1:
			print("Uso: cd..\n");
			print("Sale nella direcory superiore.\n");
		break;

		case 2:
			print("Uso: ls [directory]\n");
			print("Elenca la directory (cammino relativo) od altimenti quella corrente.\n");
		break;

		case 3:
			print("Uso: vol <volume>\n");
			print("Passa nel volume indicato.\n");
			print("Volumi disponibili:\n");
			docat("KER:\\volumes");
		break;

		case 4:
			print("Uso: cat <file>\n");
			print("Visualizza il file (cammino relativo).\n");
		break;

		case 5:
			print("Uso: exec <file>	[argomento1]	 [argomento2] ....\n");
			print("Esegue il file (cammino relativo).\n");
			print("Attenzione! Il nome del file  viene impostato automaticamente come argomento0.\n");
		break;

		case 6:
			print("Uso: execbg <file> [argomento1] [argomento2] ....\n");
			print("Esegue il file in background (cammino relativo).\n");
			print("Attenzione! Il nome del file  viene impostato automaticamente come argomento0.\n");
		break;

		case 7:
			print("Uso: kill <pid>\n");
			print("Uccide il processo il pid indicato (chiede conferma).\n");
		break;

		case 8:
			print("Uso: killall\n");
			print("Uccide tutti i processi eccetto quello della shell corrente (chiede conferma).\n");
		break;

		case 9:
			print("Uso: shutdown\n");
			print("Avvia lo shutdown (circa 15 sec);se lo shutdown e' in corso lo interrompe!\n(chiede conferma).\n");
		break;

		case 10:
			print("Uso: exit\n");
			print("Esce dalla shell.\n");
		break;

		case 11:
			print("Uso: help <comando>\n");
			print("Visualizza l'aiuto relativo al comando.\n");

			print("Comandi disponibili:\n");
			for(i=0;i<CMD_NUM;i++)
				{
				print("\t%s",comando[i]);
				if(i%7==6) print("\n");
				}
			print("\nTasti Funzione:\n");
			print("\tF9 \tMonta il Floppy\t\tF10\tSmonta il Floppy\n");
			print("\tF11\tMem Defrag     \t\tF12\tAbilita/Disabilita Log\n");
			print("\tCTRL+<numero>[+shift] cambia colore\n");

			print("\t\t1 NERO");
			print(" \t%c2 BLU   "		,CDKBLUE);
			print("\t%c3 VERDE  "	,CDKGREEN);
			print("\t%c4 CIANO\n"	,CDKCYAN);

			print("\t\t%c5 ROSSO"		,CDKRED);
			print("\t%c6 VIOLA"		,CDKPURPLE);
			print("\t%c7 MARRONE"	,CDKBROWN);
			print("\t%c8 BIANCO\n"	,CWHITE);

			print("\t\t%c9 Reverse"	,CREVERSE);
			print("%c",CREVERSE);
			print("\t0 Blink\t\tshift colori chiari\n");
		break;

		case 12:
			print("Uso: cls\n");
			print("Cancella lo schermo.\n");
		break;

		default:
			print("Ma come hai fatto?\n");
		}

	return;
	}


