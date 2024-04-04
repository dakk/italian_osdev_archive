/*
ItalianOs

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

#ifndef MIXER_H

#define MIXER_H

//generali sono le funzioni che vanno ad interagire direttamente sull'hardware
void set_mixer_reg(const unsigned short int, unsigned short int, unsigned short int);

unsigned short int get_mixer_reg(const unsigned short int, unsigned short int);

void set_mixer_regs(const unsigned short int, unsigned short int, unsigned short int,
			unsigned short int);

void get_mixer_regs(const unsigned short int, unsigned short int, unsigned short int,
			 unsigned short int *, unsigned short int *);

//resetta il mixer
void reset_mixer(const unsigned short int);

//master
void set_master_l(const unsigned short int, unsigned short int);
void set_master_r(const unsigned short int, unsigned short int);
void set_master(const unsigned short int, unsigned short int);

unsigned short int get_master_l(const unsigned short int);
unsigned short int get_master_r(const unsigned short int);
void get_master(const unsigned short int, unsigned short int *, unsigned short int *);

//voice
void set_voice_l(const unsigned short int, unsigned short int);
void set_voice_r(const unsigned short int, unsigned short int);
void set_voice(const unsigned short int, unsigned short int);

unsigned short int get_voice_l(const unsigned short int);
unsigned short int get_voice_r(const unsigned short int);
void get_voice(const unsigned short int, unsigned short int *, unsigned short int *);

//midi
void set_midi_l(const unsigned short int, unsigned short int);
void set_midi_r(const unsigned short int, unsigned short int);
void set_midi(const unsigned short int, unsigned short int);

unsigned short int get_midi_l(const unsigned short int);
unsigned short int get_midi_r(const unsigned short int);
void get_midi(const unsigned short int, unsigned short int *, unsigned short int *);

//cd
void set_cd_l(const unsigned short int, unsigned short int);
void set_cd_r(const unsigned short int, unsigned short int);
void set_cd(const unsigned short int, unsigned short int);

unsigned short int get_cd_l(const unsigned short int);
unsigned short int get_cd_r(const unsigned short int);
void get_cd(const unsigned short int, unsigned short int *, unsigned short int *);

//line
void set_line_l(const unsigned short int, unsigned short int);
void set_line_r(const unsigned short int, unsigned short int);
void set_line(const unsigned short int, unsigned short int);

unsigned short int get_line_l(const unsigned short int);
unsigned short int get_line_r(const unsigned short int);
void get_line(const unsigned short int, unsigned short int *, unsigned short int *);

//microfono
void set_microfono(const unsigned short int, unsigned short int);

unsigned short int get_microfono(const unsigned short int);

//speaker
void set_pc_speak(const unsigned short int, unsigned short int);

unsigned short int get_pc_speak(const unsigned short int);

//switch

//switch singoli
//output switch
void out_line_l_on(const unsigned short int);
void out_line_l_off(const unsigned short int);
unsigned short int out_line_l_status(const unsigned short int);

void out_line_r_on(const unsigned short int);
void out_line_r_off(const unsigned short int);
unsigned short int out_line_r_status(const unsigned short int);

void out_cd_l_on(const unsigned short int);
void out_cd_l_off(const unsigned short int);
unsigned short int out_cd_l_status(const unsigned short int);

void out_cd_r_on(const unsigned short int);
void out_cd_r_off(const unsigned short int);
unsigned short int out_cd_r_status(const unsigned short int);

void out_mic_on(const unsigned short int);
void out_mic_off(const unsigned short int);
unsigned short int out_mic_status(const unsigned short int);

//fine output switch


//input left switch
void in_l_midi_l_on(const unsigned short int);
void in_l_midi_l_off(const unsigned short int);
unsigned short int in_l_midi_l_status(const unsigned short int);

void in_l_midi_r_on(const unsigned short int);
void in_l_midi_r_off(const unsigned short int);
unsigned short int in_l_midi_r_status(const unsigned short int);

void in_l_line_l_on(const unsigned short int);
void in_l_line_l_off(const unsigned short int);
unsigned short int in_l_line_l_status(const unsigned short int);

void in_l_line_r_on(const unsigned short int);
void in_l_line_r_off(const unsigned short int);
unsigned short int in_l_line_r_status(const unsigned short int);

void in_l_cd_l_on(const unsigned short int);
void in_l_cd_l_off(const unsigned short int);
unsigned short int in_l_cd_l_status(const unsigned short int);

void in_l_cd_r_on(const unsigned short int);
void in_l_cd_r_off(const unsigned short int);
unsigned short int in_l_cd_r_status(const unsigned short int);

void in_l_mic_on(const unsigned short int);
void in_l_mic_off(const unsigned short int);
unsigned short int in_l_mic_status(const unsigned short int);
//fine input left switch


//input rigth switch
void in_r_midi_l_on(const unsigned short int);
void in_r_midi_l_off(const unsigned short int);
unsigned short int in_r_midi_l_status(const unsigned short int);

void in_r_midi_r_on(const unsigned short int);
void in_r_midi_r_off(const unsigned short int);
unsigned short int in_r_midi_r_status(const unsigned short int);

void in_r_line_l_on(const unsigned short int);
void in_r_line_l_off(const unsigned short int);
unsigned short int in_r_line_l_status(const unsigned short int);

void in_r_line_r_on(const unsigned short int);
void in_r_line_r_off(const unsigned short int);
unsigned short int in_r_line_r_status(const unsigned short int);

void in_r_cd_l_on(const unsigned short int);
void in_r_cd_l_off(const unsigned short int);
unsigned short int in_r_cd_l_status(const unsigned short int);

void in_r_cd_r_on(const unsigned short int);
void in_r_cd_r_off(const unsigned short int);
unsigned short int in_r_cd_r_status(const unsigned short int);

void in_r_mic_on(const unsigned short int);
void in_r_mic_off(const unsigned short int);
unsigned short int in_r_mic_status(const unsigned short int);
//fine input rigth switch
//fine switch singoli

//midi switch
void in_l_midi_on(const unsigned short int);
void in_l_midi_off(const unsigned short int);
void in_l_midi_status(const unsigned short int, unsigned short int *, unsigned short int *);

void in_r_midi_on(const unsigned short int);
void in_r_midi_off(const unsigned short int);
void in_r_midi_status(const unsigned short int, unsigned short int *, unsigned short int *);

void in_midi_l_on(const unsigned short int);
void in_midi_l_off(const unsigned short int);
void in_midi_l_status(const unsigned short int, unsigned short int *, unsigned short int *);

void in_midi_r_on(const unsigned short int);
void in_midi_r_off(const unsigned short int);
void in_midi_r_status(const unsigned short int, unsigned short int *, unsigned short int *);

void midi_on(const unsigned short int);
void midi_off(const unsigned short int);
void midi_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *, unsigned short int *);

//fine midi switch

//line switch

//all
void line_on(const unsigned short int);
void line_off(const unsigned short int);
void line_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *, unsigned short int *, unsigned short int *,
			unsigned short int *);

//input left
void in_l_line_on(const unsigned short int);
void in_l_line_off(const unsigned short int);
void in_l_line_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input rigth
void in_r_line_on(const unsigned short int);
void in_r_line_off(const unsigned short int);
void in_r_line_status(const unsigned short int, unsigned short int *, unsigned short int *);

//output
void out_line_on(const unsigned short int);
void out_line_off(const unsigned short int);
void out_line_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input line left
void in_line_l_on(const unsigned short int);
void in_line_l_off(const unsigned short int);
void in_line_l_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input line rigth
void in_line_r_on(const unsigned short int);
void in_line_r_off(const unsigned short int);
void in_line_r_status(const unsigned short int, unsigned short int *, unsigned short int *);

//line left
void line_l_on(const unsigned short int);
void line_l_off(const unsigned short int);
void line_l_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *);

//input line rigth
void line_r_on(const unsigned short int);
void line_r_off(const unsigned short int);
void line_r_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *);
//fine line switch

//cd switch

//all
void cd_on(const unsigned short int);
void cd_off(const unsigned short int);
void cd_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *, unsigned short int *, unsigned short int *,
			unsigned short int *);

//input left
void in_l_cd_on(const unsigned short int);
void in_l_cd_off(const unsigned short int);
void in_l_cd_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input rigth
void in_r_cd_on(const unsigned short int);
void in_r_cd_off(const unsigned short int);
void in_r_cd_status(const unsigned short int, unsigned short int *, unsigned short int *);

//output
void out_cd_on(const unsigned short int);
void out_cd_off(const unsigned short int);
void out_cd_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input cd left
void in_cd_l_on(const unsigned short int);
void in_cd_l_off(const unsigned short int);
void in_cd_l_status(const unsigned short int, unsigned short int *, unsigned short int *);

//input cd rigth
void in_cd_r_on(const unsigned short int);
void in_cd_r_off(const unsigned short int);
void in_cd_r_status(const unsigned short int, unsigned short int *, unsigned short int *);

//cd left
void cd_l_on(const unsigned short int);
void cd_l_off(const unsigned short int);
void cd_l_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *);

//input cd rigth
void cd_r_on(const unsigned short int);
void cd_r_off(const unsigned short int);
void cd_r_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *);
//fine cd switch

//mic switch

//all
void mic_on(const unsigned short int);
void mic_off(const unsigned short int);
void mic_status(const unsigned short int, unsigned short int *, unsigned short int *,
			unsigned short int *);

//input
void in_mic_on(const unsigned short int);
void in_mic_off(const unsigned short int);
void in_mic_status(const unsigned short int, unsigned short int *, unsigned short int *);

//fine mic switch
//fine switch


//input gain
void set_ingain_l(const unsigned short int, unsigned short int);
void set_ingain_r(const unsigned short int, unsigned short int);
void set_ingain(const unsigned short int, unsigned short int);

unsigned short int get_ingain_l(const unsigned short int);
unsigned short int get_ingain_r(const unsigned short int);
void get_ingain(const unsigned short int, unsigned short int *, unsigned short int *);

//output gain
void set_outgain_l(const unsigned short int, unsigned short int);
void set_outgain_r(const unsigned short int, unsigned short int);
void set_outgain(const unsigned short int, unsigned short int);

unsigned short int get_outgain_l(const unsigned short int);
unsigned short int get_outgain_r(const unsigned short int);
void get_outgain(const unsigned short int, unsigned short int *, unsigned short int *);

//agc
void agc_on(const unsigned short int);
void agc_off(const unsigned short int);
unsigned short int agc_status(const unsigned short int);

//trebble
void set_trebble_l(const unsigned short int, unsigned short int);
void set_trebble_r(const unsigned short int, unsigned short int);
void set_trebble(const unsigned short int, unsigned short int);

unsigned short int get_trebble_l(const unsigned short int);
unsigned short int get_trebble_r(const unsigned short int);
void get_trebble(const unsigned short int, unsigned short int *, unsigned short int *);

//bass
void set_bass_l(const unsigned short int, unsigned short int);
void set_bass_r(const unsigned short int, unsigned short int);
void set_bass(const unsigned short int, unsigned short int);

unsigned short int get_bass_l(const unsigned short int);
unsigned short int get_bass_r(const unsigned short int);
void get_bass(const unsigned short int, unsigned short int *, unsigned short int *);

#endif
