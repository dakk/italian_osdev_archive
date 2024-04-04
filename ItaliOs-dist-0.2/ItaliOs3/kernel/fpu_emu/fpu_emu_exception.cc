#include <fpu_emu.h>
#include <cstddef>
#include <cstdlib>

using namespace myfloat;

exception * fpu_emu_exception::get_address(){
	return coda;
}

int fpu_emu_exception::add(exceptions ex){
	exception * cursor;
	if (coda) {
			cursor= coda;
			while (cursor->next){
				cursor=cursor->next;
			};
			cursor->next= (exception *) malloc(sizeof(exception));
			if (!cursor->next) return -1;
			cursor->ex_code = ex;
			cursor = cursor->next;
			cursor->next=0;
			return 0;
	}
	else {
		coda = (exception *) malloc(sizeof(exception));
		if (!coda) return -1;
		coda->next = 0;
		coda->ex_code = ex;
		return 0;
	}
	return 0; //non verrà mai eseguito ma elimina un warning
}

int fpu_emu_exception::del(){
	exception * first;
	first = coda;
	coda = coda->next;
	free(first);
	return 0;
}

exception * fpu_emu_exception::get_first(){
	return coda;
}

exception * fpu_emu_exception::get_last(){
	exception * last;
	last=coda;
	while (!last->next){
		last=last->next;
	}
	return last;
}

fpu_emu_exception::fpu_emu_exception(){
	coda = 0;
}
