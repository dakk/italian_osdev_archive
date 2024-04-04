/* kprint.c -- Kernel Print base function */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#include <arch-ia32/videotext.h>
#include <kernel.h>
#include <vsprintf.h>
#include <stdarg.h>
#include <string.h>

#define	LOG_SIZE	4096	// log di kprint -- 4KB

char kprint_log[LOG_SIZE];	// log di kprint

void klogcat(char * str);

/* kernel print -- stampa videopage corrente, ignora la struttura delle console
 siccome questa funzione scrive direttamente in memoria video, per usarla bisogna essere in supervisor mode (CPL#0) */
int kprint(char * format,...)
{
	char buf[2048];	// mah siiiiiii!!!!!!!!! 2KB!!!!
	int len = 0;
	unsigned int i = 0;
	va_list args;
	va_start(args,format);
	len = vsprintf(buf,format,args);
	va_end(args);
	for(i=0;i<len;i++) videotext_putc(buf[i],VDTXT_DEFAULT_ATTR);
	/* accoda la stringa al log */
	klogcat(buf);
	return len;
}

/* stampa il log di kprint (usare questa, se si stampa il log con kprint fa un casino!!!) */
void print_klog(void)
{
	unsigned int i;
	for(i=0;i<strlen(kprint_log);i++) videotext_putc(kprint_log[i],VDTXT_DEFAULT_ATTR);
}

/* accoda la stringa <str> al log di kprint
 se è stata raggiunta la dimensione massima del log, lo shifta per farci entrare la stringa */
void klogcat(char * str)
{
	/* scala il buffer se necessario */
	if((strlen(kprint_log)+strlen(str)) > LOG_SIZE)
	 memcpy((void *)kprint_log,(const void *)(kprint_log+strlen(str)),LOG_SIZE-strlen(str));
	/* accoda la stringa al log */
	strcat(kprint_log,str);
}
