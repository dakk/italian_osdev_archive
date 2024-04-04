#ifndef __SIGNAL_H
#define __SIGNAL_H

#include <mydef.h>
#include <task.h>

void sig_sleep(pid_t);
void sig_wakeup(pid_t);
void sig_kill(pid_t);
#endif
