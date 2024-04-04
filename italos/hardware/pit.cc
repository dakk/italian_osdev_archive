
 /*ItaliOs
  * 
  * Copyright  (c) <2003> Abruzzo Silvio
  * 
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 7
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  */

#include <pit.h>
#include <video.h>
#include <io.h>

pit_count::pit_count(frequenza freq, canale chan, mode funzionamento,
		     access_mode acm, count_mode cm)
{
	set_frequenza(freq);
	set_canale(chan);
	set_mode(funzionamento);
	set_access_mode(acm);
	set_count_mode(cm);
}

void pit_count::set_canale(canale chan)
{
	registro_di_controllo.reg.sc = chan;
	porta_di_lavoro = registro_di_controllo.reg.sc + base_counter;
	//kout << "devo lavorare con la porta" << porta_di_lavoro << endl;
}

void pit_count::set_count_mode(count_mode cm)
{
	registro_di_controllo.reg.count = cm;
}

void pit_count::set_access_mode(access_mode acsmod)
{
	registro_di_controllo.reg.rw = acsmod;
}

void pit_count::set_mode(mode funzionamento)
{
	registro_di_controllo.reg.mode = funzionamento;
}

void pit_count::carica()
{
	outportb(control, registro_di_controllo.preg);
	//kout << "Il registro di controllo ha un valore pari a: " << int(registro_di_controllo.preg) << endl;
	//kout << "il diviso invece vale: " << int(_freq) << "Che viene frazionato in "<< int(_freq & 0xFF) << " e in " << int(_freq>>8) << endl;
	//kout << "Il valore di _freq è: " << _freq << endl;
	outportb_p(porta_di_lavoro, _freq & 0xFF);
	outportb_p(porta_di_lavoro, _freq >> 8);
}

frequenza pit_count::set_frequenza(frequenza freq)
{
	_freq = pit_freq / freq;
	return _freq;
}
