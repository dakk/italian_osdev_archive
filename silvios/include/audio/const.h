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

#ifndef _AUDIO_CONST_H
#define _AUDIO_CONST_H

//costanti che servono come indirizzo di base per la lettura/scrittura
//delle porte della scheda audio (SB16 compatibile)
#define NO_CHIP 0x00
#define BASE1  0x220
#define BASE2  0x240
#define BASE3  0x260
#define BASE4  0x280
#define BASE5  0x210

#define BASEM1 0x300
#define BASEM2 0x330

//costanti di sfasamento dall'indirizzo base (nelle funzioni andrà richiamato ad esempio BASE1+DSP_RESET)

#define FM_MUSIC_STATUS_PORT                     0x00 //READ
#define FM_MUSIC_REGISTER_ADDRESS_PORT           0x00 //WRITE
#define FM_MUSIC_DATA_PORT                       0x01 //WRITE ONLY
#define ADVANCED_FM_MUSIC_STATUS_PORT            0x02 //READ
#define ADVANCED_FM_MUSIC_REGISTER_ADDRESS_PORT  0x02 //WRITE
#define ADVANCED_FM_MUSIC_DATA_PORT              0x03 //WRITE ONLY
#define MIXER_CHIP_REGISTER_ADDRESS_PORT         0x04 //WRITE ONLY
#define MIXER_CHIP_DATA_PORT                     0x05 //READ-WRITE
#define DSP_RESET                                0x06 //WRITE ONLY
#define FM_MUSIC_STATUS_PORT2                    0x08 //READ
#define FM_MUSIC_REGISTER_PORT                   0x08 //WRITE
#define FM_MUSIC_DATA_PORT2                      0x09 //WRITE ONLY
#define DSP_READ_DATA_PORT                       0x0A //READ ONLY
#define DSP_WRITE_COMMAND_DATA                   0x0C //WRITE
#define DSP_WRITE_BUFFER_STATUS                  0x0C //READ (BIT 7)
#define DSP_READ_BUFFER_STATUS                   0x0E //READ ONLY (BIT 7)
#define DSP_ACKNOWLEDGE_DMA_16                   0x0F //READ ONLY ack
#define CDROM_COMMAND_DATA_REGISTER              0x10 //READ-WRITE
#define CDROM_STATUS_REGISTER                    0x11 //READ ONLY
#define CDROM_RESET_REGISTER                     0x12 //WRITE ONLY
#define CDROM_ENABLE_REGISTER                    0x13 //WRTIE ONLY

//costanti per gli interrupt
#define NO_INT        0x00
#define INT_DMA8      0x01 //utilizzata anche per il midi
#define INT_DMA16     0x02
#define INT_MPU4O1    0x04

//costanti per i registri del mixer
//controlli volume
#define RESET                0x00  //qualsiasi numero
#define MASTER_L             0x30  //0-31 bit 7-3
#define MASTER_R             0x31  //0-31 bit 7-3
#define VOICE_L              0x32  //0-31 bit 7-3
#define VOICE_R              0x33  //0-31 bit 7-3
#define MIDI_L               0x34  //0-31 bit 7-3
#define MIDI_R               0x35  //0-31 bit 7-3
#define CD_L                 0x36  //0-31 bit 7-3
#define CD_R                 0x37  //0-31 bit 7-3
#define LINE_L               0x38  //0-31 bit 7-3
#define LINE_R               0x39  //0-31 bit 7-3
#define MIC                  0x3A  //0-31 bit 7-3
#define PC_SPEAKER          0x3B  //0-3 bit 7,6
#define OUT_MIXER_SWITCH     0x3C  //bit 4-0
	#define OUT_SW_MIC     0x01
	#define OUT_SW_CD_R    0x02
	#define OUT_SW_CD_L    0x04
	#define OUT_SW_LINE_R  0x08
	#define OUT_SW_LINE_L  0x10
#define IN_MIXER_L_SWITCH    0x3D  //bit 6-0
	#define IN_L_SW_MIC     0x01
	#define IN_L_SW_CD_R    0x02
	#define IN_L_SW_CD_L    0x04
	#define IN_L_SW_LINE_R  0x08
	#define IN_L_SW_LINE_L  0x10
	#define IN_L_SW_MIDI_R  0x20
	#define IN_L_SW_MIDI_L  0x40
#define IN_MIXER_R_SWITCH    0x3E  //bit 6-0
	#define IN_R_SW_MIC     0x01
	#define IN_R_SW_CD_R    0x02
	#define IN_R_SW_CD_L    0x04
	#define IN_R_SW_LINE_R  0x08
	#define IN_R_SW_LINE_L  0x10
	#define IN_R_SW_MIDI_R  0x20
	#define IN_R_SW_MIDI_L  0x40
#define IN_GAIN_L            0x3F  //0-3 bit 7,6
#define IN_GAIN_R            0x40  //0-3 bit 7,6
#define OUT_GAIN_L           0x41  //0-3 bit 7,6
#define OUT_GAIN_R           0x42  //0-3 bit 7,6
#define AGC                  0x43
	#define ON   0x01
	#define OFF  0x00
#define TREBBLE_L            0x44  //0-15 bit 7-4
#define TREBBLE_R            0x45  //0-15 bit 7-4
#define BASS_L               0x46  //0-15 bit 7-4
#define BASS_R               0x47  //0-15 bit 7-4

//controllo trasmissione
#define IRQ_SETTING_REGISTER       0x80
#define DMA_SETTING_REGISTER       0x81
#define INTERRUPT_STATUS_REGISTER  0x82
//fine costanti del mixer

//costanti per il midi
//sfasamenti dall'indirizzo base
#define DATA_MIDI    0x00
#define STATUS_MIDI  0x01
#define COMMAND_MIDI 0x01

//midi command
#define MIDI_RESET   0xFF
#define MIDI_WRITE_P 0x38
#define MIDI_UART_MODE 0x3F

//altre costanti midi
#define MIDI_RESETTED 0xFE
#define MIDI_WRITE   0x80
#define MIDI_READ    0x40
#define BUSY         0xC0

// altre costanti
#define DATA_AVAIBLE 0x80
#define DSP_RESETTED 0xAA


//dsp command
#define SINGLEBYTEOUT  0x10
#define SINGLECYCLEOUT 0x14
#define AUTOINITDMAOUT 0x1C
#define SINGLEBYTEIN   0x20
#define SINGLECYCLEIN  0x24
#define AUTOINITDMAIN  0x2C
#define POLLINGMIDIIN  0x30
#define INTMIDIIN      0x31
#define UARTPOLLINGIO  0x34
#define UARTINTIO      0x35
#define UARTPOLLINGIOT 0x36
#define UARTINTIOT     0x37
#define MIDIOUT        0x38
#define SETDSTTIMEK    0x40
#define SETDSOSAMRATE  0x41
#define SETDSISAMRATE  0x42
#define SETDSPBLSIZE   0x48
#define PAUSEDAC       0x80
#define HSPEAUINITDMAO 0x90
#define HSPEAUINSCDMAO 0x91
#define HSPEAUINITDMAI 0x98
#define HSPEAUINSCDMAI 0x99
#define INPUTMONO      0xA0
#define INPUTSTEREO    0xA8
#define PAUSE8DMAIO    0xD0
#define SPKON          0xD1
#define SPKOFF         0xD3
#define CONTINUE8DMAIO 0xD4
#define PAUSE16DMAIO   0xD5
#define CONT16DMAIO    0xD6
#define SPKSTATUS      0xD8
#define EX16AUINDMAIO  0xD9
#define EX8AUINDMAIO   0xDA
#define DSP_VER        0xE1


#endif
