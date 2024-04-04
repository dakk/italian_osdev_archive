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

#include <stdio.h>
#include <io.h>
#include <audio/mixer.h>
#include <audio/const.h>
#include <audio/dsp.h>


unsigned short int get_mixer_reg(const unsigned short int base,
				 unsigned short int reg)
{
	outportb(base + MIXER_CHIP_REGISTER_ADDRESS_PORT, reg);
	return inportb(base + MIXER_CHIP_DATA_PORT);
}

void set_mixer_reg(const unsigned short int base, unsigned short int reg,
		   unsigned short int value)
{
	outportb(base + MIXER_CHIP_REGISTER_ADDRESS_PORT, reg);
	outportb(base + MIXER_CHIP_DATA_PORT, value);
}

void set_mixer_regs(const unsigned short int base,
		    unsigned short int reg_l, unsigned short int reg_r,
		    unsigned short int value)
{
	set_mixer_reg(base, reg_l, value);
	set_mixer_reg(base, reg_r, value);
}

void get_mixer_regs(const unsigned short int base,
		    unsigned short int reg_l, unsigned short int reg_r,
		    unsigned short int *value_l, unsigned short int *value_r)
{
	*value_l = get_mixer_reg(base, reg_l);
	*value_r = get_mixer_reg(base, reg_r);
}

//resetta il mixer
void reset_mixer(const unsigned short int base)
{
	set_mixer_reg(base, RESET, 0xFF);
}

//master
void set_master_l(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, MASTER_L, ((value << 3) & 0xF8));
}
void set_master_r(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, MASTER_R, ((value << 3) & 0xF8));
}
void set_master(const unsigned short int base, unsigned short int value)
{
	set_master_l(base, value);
	set_master_r(base, value);
}

unsigned short int get_master_l(const unsigned short int base)
{
	return get_mixer_reg(base, MASTER_L) >> 3;
}
unsigned short int get_master_r(const unsigned short int base)
{
	return get_mixer_reg(base, MASTER_R) >> 3;
}
void get_master(const unsigned short int base, unsigned short int *left,
		unsigned short int *right)
{
	*left = get_master_l(base);
	*right = get_master_r(base);
}

//voice
void set_voice_l(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, VOICE_L, ((value << 3) & 0xF7));
}
void set_voice_r(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, VOICE_R, ((value << 3) & 0xF7));
}
void set_voice(const unsigned short int base, unsigned short int value)
{
	set_voice_l(base, value);
	set_voice_r(base, value);
}

unsigned short int get_voice_l(const unsigned short int base)
{
	return get_mixer_reg(base, VOICE_L) >> 3;
}
unsigned short int get_voice_r(const unsigned short int base)
{
	return get_mixer_reg(base, VOICE_R) >> 3;
}
void get_voice(const unsigned short int base, unsigned short int *left,
	       unsigned short int *right)
{
	*left = get_voice_l(base);
	*right = get_voice_r(base);
}

//midi
void set_midi_l(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, MIDI_L, ((value << 3) & 0xF7));
}
void set_midi_r(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, MIDI_R, ((value << 3) & 0xF7));
}
void set_midi(const unsigned short int base, unsigned short int value)
{
	set_midi_l(base, value);
	set_midi_r(base, value);
}

unsigned short int get_midi_l(const unsigned short int base)
{
	return get_mixer_reg(base, MIDI_L);
}
unsigned short int get_midi_r(const unsigned short int base)
{
	return get_mixer_reg(base, MIDI_R);
}
void get_midi(const unsigned short int base, unsigned short int *left,
	      unsigned short int *right)
{
	*left = get_midi_l(base);
	*right = get_midi_r(base);
}

//cd
void set_cd_l(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, CD_L, ((value << 3) & 0xF7));
}
void set_cd_r(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, CD_R, ((value << 3) & 0xF7));
}
void set_cd(const unsigned short int base, unsigned short int value)
{
	set_cd_l(base, value);
	set_cd_r(base, value);
}

unsigned short int get_cd_l(const unsigned short int base)
{
	return get_mixer_reg(base, CD_L);
}
unsigned short int get_cd_r(const unsigned short int base)
{
	return get_mixer_reg(base, CD_R);
}
void get_cd(const unsigned short int base, unsigned short int *left,
	    unsigned short int *right)
{
	*left = get_cd_l(base);
	*right = get_cd_r(base);
}

//line
void set_line_l(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, LINE_L, ((value << 3) & 0xF7));
}
void set_line_r(const unsigned short int base, unsigned short int value)
{
	if (value > 31)
		value = 31;
	set_mixer_reg(base, LINE_R, ((value << 3) & 0xF7));
}
void set_line(const unsigned short int base, unsigned short int value)
{
	set_line_l(base, value);
	set_line_r(base, value);
}

unsigned short int get_line_l(const unsigned short int base)
{
	return get_mixer_reg(base, LINE_L) >> 3;
}
unsigned short int get_line_r(const unsigned short int base)
{
	return get_mixer_reg(base, LINE_R) >> 3;
}
void get_line(const unsigned short int base, unsigned short int *left,
	      unsigned short int *right)
{
	*left = get_line_l(base);
	*right = get_line_r(base);
}

//microfono
void set_microfono(const unsigned short int base, unsigned short int value)
{
	set_mixer_reg(base, MIC, ((value << 3) & 0xF7));
}

unsigned short int get_microfono(const unsigned short int base)
{
	return get_mixer_reg(base, MIC) >> 3;
}

//speaker
void set_pc_speak(const unsigned short int base, unsigned short int value)
{
	if (value > 3)
		value = 3;
	set_mixer_reg(base, PC_SPEAKER, ((value << 6) & 0xA0));
}

unsigned short int get_pc_speak(const unsigned short int base)
{
	return get_mixer_reg(base, PC_SPEAKER) >> 6;
}

//switch

//switch singoli
//output switch
void out_line_l_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_LINE_L)
		       & 0x1F));
}
void out_line_l_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) &
			(!OUT_SW_LINE_L)) & 0x1F));
}
unsigned short int out_line_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_LINE_L) ==
		OUT_SW_LINE_L ? ON : OFF;
}

void out_line_r_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_LINE_R)
		       & 0x1F));
}
void out_line_r_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) &
			(!OUT_SW_LINE_R)) & 0x1F));
}
unsigned short int out_line_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_LINE_R) ==
		OUT_SW_LINE_R ? ON : OFF;
}

void out_cd_l_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_CD_L) &
		       0x1F));
}
void out_cd_l_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) &
			(!OUT_SW_CD_L))
		       & 0x1F));
}
unsigned short int out_cd_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_CD_L) ==
		OUT_SW_CD_L ? ON : OFF;
}

void out_cd_r_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_CD_R) &
		       0x1F));
}
void out_cd_r_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) &
			(!OUT_SW_CD_R))
		       & 0x1F));
}
unsigned short int out_cd_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_CD_R) ==
		OUT_SW_CD_R ? ON : OFF;
}

void out_mic_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_MIC) &
		       0x1F));
}
void out_mic_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) & (!OUT_SW_MIC))
		       & 0x1F));
}
unsigned short int out_mic_status(const unsigned short int base)
{
	return (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_MIC) ==
		OUT_SW_MIC ? ON : OFF;
}

//fine output switch


//input left switch
void in_l_midi_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_MIDI_L) & 0x7F));
}
void in_l_midi_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_MIDI_L)) & 0x7F));
}
unsigned short int in_l_midi_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_MIDI_L) ==
		IN_L_SW_MIDI_L ? ON : OFF;
}

void in_l_midi_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_MIDI_R) & 0x7F));
}
void in_l_midi_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_MIDI_R)) & 0x7F));
}
unsigned short int in_l_midi_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_MIDI_R) ==
		IN_L_SW_MIDI_R ? ON : OFF;
}

void in_l_line_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_LINE_L) & 0x7F));
}
void in_l_line_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_LINE_L)) & 0x7F));
}
unsigned short int in_l_line_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_L) ==
		IN_L_SW_LINE_L ? ON : OFF;
}

void in_l_line_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_LINE_R) & 0x7F));
}
void in_l_line_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_LINE_R)) & 0x7F));
}
unsigned short int in_l_line_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_R) ==
		IN_L_SW_LINE_R ? ON : OFF;
}

void in_l_cd_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) | IN_L_SW_CD_L)
		       & 0x7F));
}
void in_l_cd_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_CD_L)) & 0x7F));
}
unsigned short int in_l_cd_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_CD_L) ==
		IN_L_SW_CD_L ? ON : OFF;
}

void in_l_cd_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) | IN_L_SW_CD_R)
		       & 0x7F));
}
void in_l_cd_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_CD_R)) & 0x7F));
}
unsigned short int in_l_cd_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_CD_R) ==
		IN_L_SW_CD_R ? ON : OFF;
}

void in_l_mic_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) | IN_L_SW_MIC)
		       & 0x7F));
}
void in_l_mic_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!IN_L_SW_MIC)) & 0x7F));
}
unsigned short int in_l_mic_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_MIC) ==
		IN_L_SW_MIC ? ON : OFF;
}

//fine input left switch


//input rigth switch
void in_r_midi_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_MIDI_L) & 0x7F));
}
void in_r_midi_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_MIDI_L)) & 0x7F));
}
unsigned short int in_r_midi_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_MIDI_L) ==
		IN_R_SW_MIDI_L ? ON : OFF;
}

void in_r_midi_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_MIDI_R) & 0x7F));
}
void in_r_midi_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_MIDI_R)) & 0x7F));
}
unsigned short int in_r_midi_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_MIDI_R) ==
		IN_R_SW_MIDI_R ? ON : OFF;
}

void in_r_line_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_LINE_L) & 0x7F));
}
void in_r_line_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_LINE_L)) & 0x7F));
}
unsigned short int in_r_line_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_L) ==
		IN_R_SW_LINE_L ? ON : OFF;
}

void in_r_line_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_LINE_R) & 0x7F));
}
void in_r_line_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_LINE_R)) & 0x7F));
}
unsigned short int in_r_line_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_R) ==
		IN_R_SW_LINE_R ? ON : OFF;
}

void in_r_cd_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) | IN_R_SW_CD_L)
		       & 0x7F));
}
void in_r_cd_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_CD_L)) & 0x7F));
}
unsigned short int in_r_cd_l_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_CD_L) ==
		IN_R_SW_CD_L ? ON : OFF;
}

void in_r_cd_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) | IN_R_SW_CD_R)
		       & 0x7F));
}
void in_r_cd_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_CD_R)) & 0x7F));
}
unsigned short int in_r_cd_r_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_CD_R) ==
		IN_R_SW_CD_R ? ON : OFF;
}

void in_r_mic_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) | IN_R_SW_MIC)
		       & 0x7F));
}
void in_r_mic_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!IN_R_SW_MIC)) & 0x7F));
}
unsigned short int in_r_mic_status(const unsigned short int base)
{
	return (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_MIC) ==
		IN_R_SW_MIC ? ON : OFF;
}

//fine input rigth switch
//fine switch singoli

//midi switch
void in_l_midi_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_MIDI_L | IN_L_SW_MIDI_R) & 0x7F));
}
void in_l_midi_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!(IN_L_SW_MIDI_L | IN_L_SW_MIDI_R))) & 0x7F));
}
void in_l_midi_status(const unsigned short int base,
		      unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_L_SWITCH);
	*left = (stato_reg & IN_L_SW_MIDI_L) == IN_L_SW_MIDI_L ? ON : OFF;
	*right = (stato_reg & IN_L_SW_MIDI_R) == IN_L_SW_MIDI_R ? ON : OFF;
}

void in_r_midi_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_MIDI_L | IN_R_SW_MIDI_R) & 0x7F));
}
void in_r_midi_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) &
			(!(IN_R_SW_MIDI_L | IN_R_SW_MIDI_R))) & 0x7F));
}
void in_r_midi_status(const unsigned short int base,
		      unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_R_SWITCH);
	*left = (stato_reg & IN_R_SW_MIDI_L) == IN_R_SW_MIDI_L ? ON : OFF;
	*right = (stato_reg & IN_R_SW_MIDI_R) == IN_R_SW_MIDI_R ? ON : OFF;
}

void in_midi_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_MIDI_L)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_MIDI_L)
		      & 0x7F);
}
void in_midi_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_MIDI_L)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_MIDI_L)) & 0x7F);
}
void in_midi_l_status(const unsigned short int base,
		      unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_MIDI_L) ==
		IN_L_SW_MIDI_L ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_MIDI_L) ==
		IN_R_SW_MIDI_L ? ON : OFF;
}

void in_midi_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_MIDI_R)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_MIDI_R)
		      & 0x7F);
}
void in_midi_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_MIDI_R)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_MIDI_R)) & 0x7F);
}
void in_midi_r_status(const unsigned short int base,
		      unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_MIDI_L) ==
		IN_L_SW_MIDI_L ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_MIDI_L) ==
		IN_R_SW_MIDI_L ? ON : OFF;
}

void midi_on(const unsigned short int base)
{
	in_l_midi_on(base);
	in_r_midi_on(base);
}
void midi_off(const unsigned short int base)
{
	in_l_midi_off(base);
	in_r_midi_off(base);
}
void midi_status(const unsigned short int base,
		 unsigned short int *in_l_m_l,
		 unsigned short int *in_l_m_r,
		 unsigned short int *in_r_m_l, unsigned short int *in_r_m_r)
{
	in_l_midi_status(base, in_l_m_l, in_l_m_r);
	in_r_midi_status(base, in_r_m_l, in_r_m_r);
}

//fine midi switch

//line switch

//all
void line_on(const unsigned short int base)
{
	out_line_on(base);
	in_l_line_on(base);
	in_r_line_on(base);
}
void line_off(const unsigned short int base)
{
	out_line_off(base);
	in_l_line_off(base);
	in_r_line_off(base);
}
void line_status(const unsigned short int base,
		 unsigned short int *out_line_l,
		 unsigned short int *out_line_r,
		 unsigned short int *in_l_line_l,
		 unsigned short int *in_l_line_r,
		 unsigned short int *in_r_line_l,
		 unsigned short int *in_r_line_r)
{
	out_line_status(base, out_line_l, out_line_r);
	in_l_line_status(base, in_l_line_l, in_l_line_r);
	in_l_line_status(base, in_r_line_l, in_r_line_r);
}

//input left
void in_l_line_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) |
			IN_L_SW_LINE_L | IN_L_SW_LINE_R) & 0x7F));
}
void in_l_line_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!(IN_L_SW_LINE_L | IN_L_SW_LINE_R))) & 0x7F));
}
void in_l_line_status(const unsigned short int base,
		      unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_L_SWITCH);
	*left = (stato_reg & IN_L_SW_LINE_L) == IN_L_SW_LINE_L ? ON : OFF;
	*right = (stato_reg & IN_L_SW_LINE_R) == IN_L_SW_LINE_R ? ON : OFF;
}

//input rigth
void in_r_line_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_R_SWITCH) |
			IN_R_SW_LINE_L | IN_R_SW_LINE_R) & 0x7F));
}
void in_r_line_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      ((get_mixer_reg(base, IN_MIXER_L_SWITCH) &
			(!(IN_R_SW_LINE_L | IN_R_SW_LINE_R))) & 0x7F));
}
void in_r_line_status(const unsigned short int base,
		      unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_R_SWITCH);
	*left = (stato_reg & IN_R_SW_LINE_L) == IN_R_SW_LINE_L ? ON : OFF;
	*right = (stato_reg & IN_R_SW_LINE_R) == IN_R_SW_LINE_R ? ON : OFF;
}

//output
void out_line_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_LINE_L
			| OUT_SW_LINE_R) & 0x1F));
}
void out_line_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      ((get_mixer_reg(base, OUT_MIXER_SWITCH) &
			(!(OUT_SW_LINE_L | OUT_SW_LINE_R))) & 0x1F));
}
void out_line_status(const unsigned short int base,
		     unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, OUT_MIXER_SWITCH);
	*left = (stato_reg & IN_L_SW_LINE_L) == OUT_SW_LINE_L ? ON : OFF;
	*right = (stato_reg & IN_L_SW_LINE_R) == OUT_SW_LINE_R ? ON : OFF;
}

//input line left
void in_line_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_LINE_L)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_LINE_L)
		      & 0x7F);
}
void in_line_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_LINE_L)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_LINE_L)) & 0x7F);
}
void in_line_l_status(const unsigned short int base,
		      unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_L) ==
		IN_L_SW_LINE_L ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_L) ==
		IN_R_SW_LINE_L ? ON : OFF;
}

//input line rigth
void in_line_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_LINE_R)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_LINE_R)
		      & 0x7F);
}
void in_line_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_LINE_R)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_LINE_R)) & 0x7F);
}
void in_line_r_status(const unsigned short int base,
		      unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_R) ==
		IN_L_SW_LINE_R ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_R) ==
		IN_R_SW_LINE_R ? ON : OFF;
}

//line left
void line_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_LINE_L)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_LINE_L)
		      & 0x7F);
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_LINE_L)
		      & 0x1F);
}
void line_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_LINE_L)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_LINE_L)) & 0x7F);
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) &
		       (!OUT_SW_LINE_L)) & 0x1F);
}
void line_l_status(const unsigned short int base, unsigned short int *out,
		   unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_L) ==
		IN_L_SW_LINE_L ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_L) ==
		IN_R_SW_LINE_L ? ON : OFF;
	*out = (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_LINE_L) ==
		OUT_SW_LINE_L ? ON : OFF;
}

//input line rigth
void line_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       IN_L_SW_LINE_R)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       IN_R_SW_LINE_R)
		      & 0x7F);
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) | OUT_SW_LINE_R)
		      & 0x1F);
}
void line_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_LINE_R)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_LINE_R)) & 0x7F);
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) &
		       (!OUT_SW_LINE_R)) & 0x1F);
}
void line_r_status(const unsigned short int base, unsigned short int *out,
		   unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_LINE_R) ==
		IN_L_SW_LINE_R ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_LINE_R) ==
		IN_R_SW_LINE_R ? ON : OFF;
	*out = (get_mixer_reg(base, OUT_MIXER_SWITCH) & OUT_SW_LINE_R) ==
		OUT_SW_LINE_R ? ON : OFF;
}

//fine line switch

//cd switch

//all
void cd_on(const unsigned short int base)
{
	out_cd_on(base);
	in_l_cd_on(base);
	in_r_cd_on(base);
}
void cd_off(const unsigned short int base)
{
	out_cd_off(base);
	in_l_cd_off(base);
	in_r_cd_off(base);
}
void cd_status(const unsigned short int base, unsigned short int *out_cd_l,
	       unsigned short int *out_cd_r, unsigned short int *in_l_cd_l,
	       unsigned short int *in_l_cd_r,
	       unsigned short int *in_r_cd_l, unsigned short int *in_r_cd_r)
{
	out_cd_status(base, out_cd_l, out_cd_r);
	in_l_cd_status(base, in_l_cd_l, in_l_cd_r);
	in_r_cd_status(base, in_r_cd_l, in_r_cd_r);
}

//input left
void in_l_cd_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) |
		       (IN_L_SW_CD_L | IN_L_SW_CD_R)) & 0x7F);
}
void in_l_cd_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!(IN_L_SW_CD_L | IN_L_SW_CD_R))) & 0x7F);
}
void in_l_cd_status(const unsigned short int base,
		    unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_L_SWITCH);
	*left = (stato_reg & IN_L_SW_CD_L) == IN_L_SW_CD_L ? ON : OFF;
	*right = (stato_reg & IN_L_SW_CD_R) == IN_L_SW_CD_R ? ON : OFF;
}

//input rigth
void in_r_cd_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) |
		       (IN_R_SW_CD_L | IN_R_SW_CD_R)) & 0x7F);
}
void in_r_cd_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!(IN_R_SW_CD_L | IN_R_SW_CD_R))) & 0x7F);
}
void in_r_cd_status(const unsigned short int base,
		    unsigned short int *left, unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, IN_MIXER_R_SWITCH);
	*left = (stato_reg & IN_R_SW_CD_L) == IN_R_SW_CD_L ? ON : OFF;
	*right = (stato_reg & IN_R_SW_CD_R) == IN_R_SW_CD_R ? ON : OFF;
}

//output
void out_cd_on(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) |
		       (OUT_SW_CD_L | OUT_SW_CD_R)) & 0x1F);
}
void out_cd_off(const unsigned short int base)
{
	set_mixer_reg(base, OUT_MIXER_SWITCH,
		      (get_mixer_reg(base, OUT_MIXER_SWITCH) &
		       (!(OUT_SW_CD_L | OUT_SW_CD_R))) & 0x1F);
}
void out_cd_status(const unsigned short int base, unsigned short int *left,
		   unsigned short int *right)
{
	unsigned short int stato_reg;
	stato_reg = get_mixer_reg(base, OUT_MIXER_SWITCH);
	*left = (stato_reg & OUT_SW_CD_L) == OUT_SW_CD_L ? ON : OFF;
	*right = (stato_reg & OUT_SW_CD_R) == OUT_SW_CD_R ? ON : OFF;
}

//input cd left
void in_cd_l_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) | IN_L_SW_CD_L)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) | IN_R_SW_CD_L)
		      & 0x7F);
}
void in_cd_l_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_CD_L)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_CD_L)) & 0x7F);
}
void in_cd_l_status(const unsigned short int base,
		    unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_CD_L) ==
		IN_L_SW_CD_L ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_CD_L) ==
		IN_R_SW_CD_L ? ON : OFF;
}

//input cd rigth
void in_cd_r_on(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) | IN_L_SW_CD_R)
		      & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) | IN_R_SW_CD_R)
		      & 0x7F);
}
void in_cd_r_off(const unsigned short int base)
{
	set_mixer_reg(base, IN_MIXER_L_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_L_SWITCH) &
		       (!IN_L_SW_CD_R)) & 0x7F);
	set_mixer_reg(base, IN_MIXER_R_SWITCH,
		      (get_mixer_reg(base, IN_MIXER_R_SWITCH) &
		       (!IN_R_SW_CD_R)) & 0x7F);
}
void in_cd_r_status(const unsigned short int base,
		    unsigned short int *in_l, unsigned short int *in_r)
{
	*in_l = (get_mixer_reg(base, IN_MIXER_L_SWITCH) & IN_L_SW_CD_R) ==
		IN_L_SW_CD_R ? ON : OFF;
	*in_r = (get_mixer_reg(base, IN_MIXER_R_SWITCH) & IN_R_SW_CD_R) ==
		IN_R_SW_CD_R ? ON : OFF;
}

//cd left
void cd_l_on(const unsigned short int base)
{
	out_cd_l_on(base);
	in_cd_l_on(base);
}
void cd_l_off(const unsigned short int base)
{
	out_cd_l_off(base);
	in_cd_l_off(base);
}
void cd_l_status(const unsigned short int base, unsigned short int *out,
		 unsigned short int *in_l, unsigned short int *in_r)
{
	*out = out_cd_l_status(base);
	in_cd_l_status(base, in_l, in_r);
}

//cd rigth
void cd_r_on(const unsigned short int base)
{
	out_cd_r_on(base);
	in_cd_r_on(base);
}
void cd_r_off(const unsigned short int base)
{
	out_cd_r_off(base);
	in_cd_r_off(base);
}
void cd_r_status(const unsigned short int base, unsigned short int *out,
		 unsigned short int *in_l, unsigned short int *in_r)
{
	*out = out_cd_r_status(base);
	in_cd_r_status(base, in_l, in_r);
}

//fine cd switch

//mic switch

//all
void mic_on(const unsigned short int base)
{
	out_mic_on(base);
	in_mic_on(base);
}
void mic_off(const unsigned short int base)
{
	out_mic_off(base);
	in_mic_off(base);
}
void mic_status(const unsigned short int base, unsigned short int *out,
		unsigned short int *in_l, unsigned short int *in_r)
{
	*out = out_mic_status(base);
	in_mic_status(base, in_l, in_r);
}

//input
void in_mic_on(const unsigned short int base)
{
	in_l_mic_on(base);
	in_r_mic_on(base);
}
void in_mic_off(const unsigned short int base)
{
	in_l_mic_off(base);
	in_r_mic_off(base);
}
void in_mic_status(const unsigned short int base, unsigned short int *in_l,
		   unsigned short int *in_r)
{
	*in_l = in_l_mic_status(base);
	*in_r = in_r_mic_status(base);
}

//fine mic switch
//fine switch


//input gain
void set_ingain_l(const unsigned short int base, unsigned short int value)
{
	if (value > 3)
		value = 3;
	set_mixer_reg(base, IN_GAIN_L, ((value << 6) & 0xA0));
}
void set_ingain_r(const unsigned short int base, unsigned short int value)
{
	if (value > 3)
		value = 3;
	set_mixer_reg(base, IN_GAIN_R, ((value << 6) & 0xA0));
}
void set_ingain(const unsigned short int base, unsigned short int value)
{
	set_ingain_l(base, value);
	set_ingain_r(base, value);
}

unsigned short int get_ingain_l(const unsigned short int base)
{
	return get_mixer_reg(base, IN_GAIN_L) >> 6;
}
unsigned short int get_ingain_r(const unsigned short int base)
{
	return get_mixer_reg(base, IN_GAIN_R) >> 6;
}
void get_ingain(const unsigned short int base, unsigned short int *left,
		unsigned short int *right)
{
	*left = get_ingain_l(base);
	*right = get_ingain_r(base);
}

//output gain
void set_outgain_l(const unsigned short int base, unsigned short int value)
{
	if (value > 3)
		value = 3;
	set_mixer_reg(base, OUT_GAIN_L, ((value << 6) & 0xA0));
}
void set_outgain_r(const unsigned short int base, unsigned short int value)
{
	if (value > 3)
		value = 3;
	set_mixer_reg(base, OUT_GAIN_R, ((value << 6) & 0xA0));
}
void set_outgain(const unsigned short int base, unsigned short int value)
{
	set_outgain_l(base, value);
	set_outgain_r(base, value);
}

unsigned short int get_outgain_l(const unsigned short int base)
{
	return get_mixer_reg(base, OUT_GAIN_L) >> 6;
}
unsigned short int get_outgain_r(const unsigned short int base)
{
	return get_mixer_reg(base, OUT_GAIN_R) >> 6;
}
void get_outgain(const unsigned short int base, unsigned short int *left,
		 unsigned short int *right)
{
	*left = get_outgain_l(base);
	*right = get_outgain_r(base);
}

//agc
void agc_on(const unsigned short int base)
{
	set_mixer_reg(base, AGC, ON);
}
void agc_off(const unsigned short int base)
{
	set_mixer_reg(base, AGC, OFF);
}
unsigned short int agc_status(const unsigned short int base)
{
	return (get_mixer_reg(base, AGC) & 0x01);
}

//trebble
void set_trebble_l(const unsigned short int base, unsigned short int value)
{
	if (value > 15)
		value = 15;
	set_mixer_reg(base, TREBBLE_L, ((value << 4) & 0xF0));
}
void set_trebble_r(const unsigned short int base, unsigned short int value)
{
	if (value > 15)
		value = 15;
	set_mixer_reg(base, TREBBLE_R, ((value << 4) & 0xF0));
}
void set_trebble(const unsigned short int base, unsigned short int value)
{
	set_trebble_l(base, value);
	set_trebble_r(base, value);
}

unsigned short int get_trebble_l(const unsigned short int base)
{
	return get_mixer_reg(base, TREBBLE_L) >> 4;
}
unsigned short int get_trebble_r(const unsigned short int base)
{
	return get_mixer_reg(base, TREBBLE_R) >> 4;
}
void get_trebble(const unsigned short int base, unsigned short int *left,
		 unsigned short int *right)
{
	*left = get_trebble_l(base);
	*right = get_trebble_r(base);
}

//bass
void set_bass_l(const unsigned short int base, unsigned short int value)
{
	if (value > 15)
		value = 15;
	set_mixer_reg(base, BASS_L, ((value << 4) & 0xF0));
}
void set_bass_r(const unsigned short int base, unsigned short int value)
{
	if (value > 15)
		value = 15;
	set_mixer_reg(base, BASS_R, ((value << 4) & 0xF0));
}
void set_bass(const unsigned short int base, unsigned short int value)
{
	set_bass_l(base, value);
	set_bass_r(base, value);
}

unsigned short int get_bass_l(const unsigned short int base)
{
	return get_mixer_reg(base, BASS_L) >> 4;
}
unsigned short int get_bass_r(const unsigned short int base)
{
	return get_mixer_reg(base, BASS_R) >> 4;
}
void get_bass(const unsigned short int base, unsigned short int *left,
	      unsigned short int *right)
{
	*left = get_bass_l(base);
	*right = get_bass_r(base);
}
