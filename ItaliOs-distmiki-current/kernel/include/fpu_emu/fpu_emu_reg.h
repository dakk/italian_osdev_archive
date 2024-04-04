#ifdef __FPU_EMU__

#ifndef __FPU_EMU_REG__
#define __FPU_EMU_REG__

  #include <stddef.h>
  #include <fpu_type.h>
  
  using namespace myfloat;
  

typedef struct reg {
	word cr;
	word sr;
	word tr;
	qword fop:48;
	qword fip:48;
	word opcode:11;
	my_double r[8];
} reg;

typedef struct rs {
	word cr;
	word sr;
	word tr;
	qword fop:48;
	qword fip:48;
	word opcode:11;
} reg_env;

class fpu_emu_reg{
	//variabili proprie della FPU
    private:
	qword FIP;
	qword FOP;
	word CR;
	word SR;
	word TR;
	word OPCODE;
	//membri aggiuntivi
    public:
    	//metodi per la lettura/scrittura dei registri
	my_double R[8];
	qword get_fip(void);
	void set_fip(qword);
	qword get_fop(void);
	void set_fop(qword);
	word get_cr(void);
	void set_cr(word);
	word get_sr(void);
	void set_sr(word);
	word get_tr(void);
	void set_tr(word);
	word get_opcode(void);
	void set_opcode(word);
    //protected:
  };

#endif //__FPU_EMU_REG__

#endif //__FPU_EMU__
