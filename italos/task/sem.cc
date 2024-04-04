#include <sem.h>
#include <task.h>

/*
class sem{
	public:
		void init();
		void prendi();
		void lascia();
	private:
		size_t _valore;
};*/

#define PRESO  1
#define LIBERO !PRESO

sem::sem()
{
	init();
}

void sem::init()
{
	_valore = LIBERO;
}

void sem::prendi()
{
	while (1) {
		asm("cli");	//disattivo gli interrupt
		if (_valore == PRESO) {
			//aspettiamo 5 centesimi di secondo prima di riprovare
			asm("sti");
			sleep(5);
		} else {
			_valore = PRESO;
			asm("sti");
			break;
		}
	}
}

void sem::lascia()
{
	asm("cli");		//disattivo gli interrupt
	_valore = LIBERO;
	asm("sti");
}
