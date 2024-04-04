/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cstddef>
#ifndef endl
#define  endl  '\n'
#endif

enum{
	ON, OFF
};

class Debug{
public:
        Debug();
        void put(const char c) ;
	void put(const char* c) ;
	Debug& operator << (const int numero);
	Debug& operator << (const unsigned int numero);
	Debug& operator << (const char* str);
	Debug& operator << (const char ch);
	void istruction_trace(word status);
	void register_trace(word status);
	void stop();
};

extern Debug debug;

#endif
