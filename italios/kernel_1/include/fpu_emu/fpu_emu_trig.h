#ifdef __FPU_EMU__

#ifndef __FPU_EMU_TRIG__
#define __FPU_EMU_TRIG__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>
  #include <fpu_trig_value.h>

	class fpu_emu_trig{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	    my_double VET_SIN[24];
	    my_double VET_COS[24];
	    my_double NEAR[24];
	    my_double VETT[24];
	    long int fattoriale(long int);
	    my_double converti(int);
	    int fpu_emu_trig::get_nearest(void);
	  public:
	    void fsin(void);
	    void fcos(void);
	    void fsincos(void);
	    void fptan();
	    void fpatan();
	    void fpu_emu_trig_init(fpu_emu_reg *,fpu_emu_exception *);
 	};

#endif //__FPU_EMU_TRIG__
#endif //__FPU_EMU__
