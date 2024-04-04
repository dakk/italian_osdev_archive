#include <fpu_emu.h>
#include <cstddef>

fpu_emu::fpu_emu(){
	math.fpu_emu_math_init(&regi,&exce);
	trig.fpu_emu_trig_init(&regi,&exce);
	mov.fpu_emu_mov_init(&regi,&exce);
	comp.fpu_emu_comp_init(&regi,&exce);
	data.fpu_emu_data_init(&regi,&exce);
	control.fpu_emu_control_init(&regi,&exce);
	exp.fpu_emu_exp_init(&regi,&exce);
	control.fninit();
	return;
}
