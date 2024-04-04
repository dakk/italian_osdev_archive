/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: vbe.h			|
	| Date: 30/01/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

#ifndef _HARDWARE_DRIVER_VBE_H_
#define _HARDWARE_DRIVER_VBE_H_

#include <kdef.h>

namespace hardware{

	namespace driver{
	
		namespace video{
			
			struct VBEModeInfoBlock {
				unsigned short  mode_attributes;
				unsigned char   win_a_atributes;
				unsigned char   win_b_attributes;
				unsigned short  win_granularity;
				unsigned short  win_size;
				unsigned short  win_a_segment;
				unsigned short  win_b_segment;
				unsigned int    win_func_ptr;
				unsigned short  bytes_per_scanline;
				unsigned short  x_resolution;
				unsigned short  y_resolution;
				unsigned char   x_char_size;
				unsigned char   y_char_size;
				unsigned char   number_of_planes;
				unsigned char   bits_per_pixel;
				unsigned char   number_of_banks;
				unsigned char   memory_model;
				unsigned char   bank_size;
				unsigned char   number_of_image_pages;
				unsigned char   reserved;
				unsigned char   red_mask_size;
				unsigned char   red_field_position;
				unsigned char   green_mask_size;
				unsigned char   green_field_position;
				unsigned char   blue_mask_size;
				unsigned char   blue_field_position;
				unsigned char   reserved_mask_size;
				unsigned char   reserved_field_position;
				unsigned char   direct_color_mode_info;
				unsigned int    physical_base_ptr;
				unsigned int    offscreen_memory_offset;
				unsigned short  offscreen_memory_size;
			} __attribute__((packed));
                	
			struct VBESVGAInfoBlock {
				unsigned char   signature[4];      /* VESA */
				unsigned short  version;
				char*           oem_string_ptr;
				unsigned char   capabilities[4];
				unsigned short* video_mode_ptr;
				unsigned short  total_memory;
				unsigned short  oem_software_rev;
				char*           oem_vendor_name_ptr;
				char*           oem_product_name_ptr;
				char*           oem_product_rev_ptr;
				/* Reserved data here */
			} __attribute__((packed));

                	class VBE{
			public:
				//Inizializzo i driver VBE
				static bool Init(unsigned long info_mode_addr);

				//Puntatori
				static void (*Clear)(char r, char g, char b);
				static void (*DrawFillRectangle)(int x, int y, int width, int height, char r, char g, char b);
				static void (*DrawImage)(int width, int height, const unsigned char *pixeldata);

				//Pulisco lo schermo utilizzando un colore.
				static void Clear24(char r, char g, char b);
				static void Clear32(char r, char g, char b);

				//Disegno un rettangolo pieno.
				static void DrawFillRectangle24(int x, int y, int width, int height, char r, char g, char b);
				static void DrawFillRectangle32(int x, int y, int width, int height, char r, char g, char b);

				//Disegno un'ìmmagine.
				static void DrawImage24(int width, int height, const unsigned char *pixeldata);
				static void DrawImage32(int width, int height, const unsigned char *pixeldata);

				//Scrivo un carattere
				//static void Print(const char c);
				//static void Print(const char *c);
				static unsigned char *vbevideomem; //Memoria video
				//static unsigned dword *vbevideomem32; //Memoria video a 32 bit
				static unsigned int bpp;

				static void ReInit();
			private:
				static VBEModeInfoBlock *vbeinfo; //Blocco informazioni
			};
		}
	}
}

#endif
