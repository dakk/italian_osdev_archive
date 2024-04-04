#include <fpu_emu.h>
#include <cstddef>


using namespace myfloat;

void fpu_emu_control::fninit(void){
  //inizializzazione della fpu
  registri->set_sr(fpu_const::INIT_SR);
  registri->set_tr(fpu_const::INIT_TR);
  registri->set_cr(fpu_const::INIT_CR);
  registri->set_fip(0);
  registri->set_fop(0);
  registri->set_opcode(0);
  return;
}

void fpu_emu_control::finit(void){
  fninit();
  return;
}

void fpu_emu_control::fnclex(void){
  //pulizia dei flag del registro di stato
  registri->set_sr(registri->get_sr() & fpu_const::CLEX);
  return;
}

void fpu_emu_control::fclex(void){
  //controlla che non ci siano eccezzioni ancora da eseguire prima di fare la pulizia dei flag
  fnclex();
  return;
}

void fpu_emu_control::fldcw(unsigned short int source){
  registri->set_cr(source);
  return;
}

void fpu_emu_control::fstcw(unsigned short int * destination){
  *destination=registri->get_cr();
  return;
}

void fpu_emu_control::fnstcw(unsigned short int * destination){
  *destination = registri->get_cr();
  return;
}

void fpu_emu_control::fstsw(unsigned short int * destination){
  *destination = registri->get_sr();
  return;
}

void fpu_emu_control::fnstsw(unsigned short int * destination){
  *destination = registri->get_sr();
  return;
}

void fpu_emu_control::fldenv(reg_env * regi) {
  registri->set_cr(regi->cr);
  registri->set_sr(regi->sr);
  registri->set_tr(regi->sr);
  registri->set_fop(regi->fop);
  registri->set_fip(regi->fip);
  registri->set_opcode(regi->opcode);
  return;
}

void fpu_emu_control::fnstenv(reg_env * regi) {
  regi->cr  = registri->get_cr();
  regi->sr  = registri->get_sr();
  regi->tr  = registri->get_tr();
  regi->fop = registri->get_fop();
  regi->fip = registri->get_fip();
  regi->opcode =registri->get_opcode();
  return;
}

void fpu_emu_control::fstenv(reg_env * regi) {
  fnstenv(regi);
  return;
}

void fpu_emu_control::ffree(unsigned short int tag){
  int top = (registri->get_sr() & 0x3800) >> 11;
  //calcolo del registro assoluto
  top += tag;
  if (top>7) {
  	top-=7;
  }
  //settaggio del registro
  registri->set_tr(registri->get_tr() | (fpu_const::TR_EMPTY << (2*top)));
  return;
}

void fpu_emu_control::fwait(void){
  asm("wait");
  return;
}

void fpu_emu_control::fnop(void){
  asm("nop");
  return;
}

void fpu_emu_control::fpu_emu_control_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
  registri=regi;
  exce=exc;
  return;
}

void fpu_emu_control::fdecstp(void){
  int top = (registri->get_sr() & 0x3800) >> 11;
  if (top==0) top=7;
  else top-=1;
  registri->set_sr((registri->get_sr() & !0x3800) | (top << 11));
  return;
}

void fpu_emu_control::fincstp(void){
  int top = (registri->get_sr() & 0x3800) >> 11;
  if (top==7) top=0;
  else top+=1;
  registri->set_sr((registri->get_sr() & !0x3800) | (top << 11));
  return;
}

void fpu_emu_control::fsave(reg * regi){
  fnsave(regi);
  return;
}

void fpu_emu_control::fnsave(reg * regi){
  int i;
  regi->cr  = registri->get_cr();
  regi->sr  = registri->get_sr();
  regi->tr  = registri->get_tr();
  regi->fop = registri->get_fop();
  regi->fip = registri->get_fip();
  regi->opcode =registri->get_opcode();
  for (i=0;i<8;i++)
    regi->r[i].strutturato = registri->R[i].strutturato;
  fninit();
  return;
}

void fpu_emu_control::frstor(reg * regi){
  int i;
  registri->set_cr(regi->cr);
  registri->set_sr(regi->sr);
  registri->set_tr(regi->sr);
  registri->set_fop(regi->fop);
  registri->set_fip(regi->fip);
  registri->set_opcode(regi->opcode);
  for (i=0;i<8;i++)
    registri->R[i].strutturato=regi->r[i].strutturato;
  return;
}
