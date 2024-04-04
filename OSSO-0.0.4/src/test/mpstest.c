#include <printf.h>
#include <processes.h>
#include <protocols/mpsint.h>
#include <protocols/prg2mps.h>
#include <protocols/drv2mps.h>
#include <messages.h>

int main(void) {
  MpsRetVal errcode;
  long pid;
  void * msg;
  msginfo info;

  msg=recv_msg(&info);
  if (((union drvmps_msg *)msg)->intest.Type!=DRVMPS_START) {
    printf("[MPS] ***** ERROR: message received not START!!!\n");
    while (1);
  }

  for (pid=0; pid<100000000; pid++);
  printf("[MPSTEST] Started with pid %d\n",getpid());
  errcode=GetDriverPid("io_device","keyboard",&pid,MPS_WAITRUN);
  if (errcode) {
    printf("[MPSTEST] ***** Error in getting data: %s!!!\n",
      MpsStrerror(errcode));
    exit();
  }
  printf("[MPSTEST] keyboard pid is %d\n",pid);
  return 0;
}
