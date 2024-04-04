#ifndef _SEM_H_
#define  _SEM_H_

#include <stddef.h>

class sem{
	public:
		sem();
		void init();
		void prendi();
		void lascia();
	private:
		volatile size_t _valore;
};

#endif
