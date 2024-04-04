/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <gdt.h>
#include <task.h>
#include <mem.h>
#include <register.h>

#define MAX_TASK 50

unsigned int farjump(unsigned short int selector, unsigned int offset);

void load_tr(unsigned int TSSselector);

struct TSS task[MAX_TASK];
struct TASK task_info[MAX_TASK];
unsigned int task_desk[MAX_TASK];

unsigned int num_task = 0;
unsigned int cur_task = 0;
unsigned int active = 0;

void add_task(void *eip, unsigned char *nome)
{
	task[num_task].trapbit = 0;
	task[num_task].iomapbase = sizeof(struct TSS);
	task[num_task].fs = task[num_task].gs = 0;	/* fs=gs=0 */
	task[num_task].ds = get_ds();
	task[num_task].es = task[num_task].ss = get_ss();
	task[num_task].cs = get_cs();
	task[num_task].eflags = 0x3202L;
	task[num_task].esp = (unsigned int) malloc(0x3000);
	task[num_task].eip = (unsigned int) eip;
	task[num_task].ldt = 0;
	task[num_task].eax = 0;
	task[num_task].ebx = 0;
	task[num_task].ecx = 0;
	task[num_task].edx = 0;
	task[num_task].ebp = (unsigned int) malloc(0x3000);
	task_desk[num_task] =
		add_desc_to_gdt((unsigned int) &task[num_task], 103,
				(D_TSS + D_BIG));
	strcpy(task_info[num_task].nome, nome);
	task_info[num_task].pid = num_task;
	task_info[num_task].stato = AVVIABILE;
//      debug("\nadd_task: task %d creato il suo descrittore vale %d e il suo eip vale %d", num_task, task_desk[num_task], task[num_task].eip);
	num_task++;
	//getch();
	return;
}

void ps()
{
	int i;
	unsigned char *stato = malloc(sizeof(unsigned char) * 30);
	printf("\nPID     STATO             NOME");
	for (i = 1; i < num_task; i++) {
		switch (task_info[i].stato) {
		case 0:
			strcpy(stato, "ESEGUIBILE");
			break;
		case 1:
			strcpy(stato, "MORTO");
			break;
		case 2:
			strcpy(stato, "ADDORMENTATO");
			break;
		default:
			strcpy(stato, "SCONOSCIUTO");
		}
		printf("\n%d     %s        %s", task_info[i].pid, stato,
		       task_info[i].nome);
	}
	free(stato);
	return;
}

void suicide()
{
	omicide(cur_task);
}


void omicide(unsigned short int pid)
{
	//free(task[pid].ebp );
	//free(task[pid].esp );
	task_info[pid].stato = MORTO;
	while (1);
}

void init_task()
{
	load_tr(task_desk[0]);
	printf("\ninit_task: TR caricato");
	active = 1;
	printf("\ninit_task: scheduler attivo");
	return;
}


void scheduler()
{
	if (active) {

		do
			if (++cur_task >= num_task)
				cur_task = 0;
		while (task_info[cur_task].stato != AVVIABILE);


		//printf("\nTask corrente = %d", cur_task);

		farjump(task_desk[cur_task], 0);
	}
}


void get_curtask_status()
{
	printf("\nPID = %d, EAX = 0x%x, EBX = 0x%x, ECX = 0x%x, EDX = 0x%x",
	       cur_task, task[cur_task].eax, task[cur_task].ebx,
	       task[cur_task].ecx, task[cur_task].edx);
	printf("\nCS = 0x%x, DS = 0x%x, SS = 0x%x, ES = 0x%x, FS = 0x%x, GS = 0x%x", task[cur_task].cs, task[cur_task].ds, task[cur_task].ss, task[cur_task].es, task[cur_task].fs, task[cur_task].gs);
	printf("\nESP = 0x%x, EBP = 0x%x,  EIP = 0x%x, CR3 = 0x%x, EFLAG = 0x%x", task[cur_task].esp, task[cur_task].ebp, task[cur_task].cr3, task[cur_task].eflags);
	return;
}

unsigned int prendi_pid_task_corrente(void)
{
	return cur_task;
}
