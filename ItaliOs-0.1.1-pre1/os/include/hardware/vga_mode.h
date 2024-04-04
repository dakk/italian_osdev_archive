#ifndef _VGA_MODE_H_
#define _VGA_MODE_H_

unsigned char mode13h[62] = {
	// MISC reg,  STATUS reg,    SEQ regs
	0x63, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x0E,
	// CRTC regs
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0BF, 0x1F, 0x00, 0x41, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0x0E3, 0xFF,
	// GRAPHICS regs
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
	// ATTRIBUTE CONTROLLER regs
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B,
	0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};


/*  Same as Mode 01H */
unsigned char mode00h[62] = {
/*  MISC reg,  STATUS reg,    SEQ regs */
   0x67,      0x00,          0x03,0x08,0x03,0x00,0x02,
/*  CRTC regs */
0x2D,0x27,0x28,0x90,0x2B,0xA0,0xBF,0x1F,0x00,0x4F,0x06,0x07,0x00,0x00,0x00,
0x31,0x9C,0x8E,0x8F,0x14,0x1F,0x96,0xB9,0xA3,0xFF,
/*  GRAPHICS regs */
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF,
/*  ATTRIBUTE CONTROLLER regs */
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x08,0x00
};

/*  Same as Mode 02H */
unsigned char mode03h[62] = {
/*  MISC reg,  STATUS reg,    SEQ regs */
   0x67,      0x00,          0x03,0x00,0x03,0x00,0x02,
/*  CRTC regs */
0x5F,0x4F,0x50,0x82,0x55,0x81,0xBF,0x1F,0x00,0x4F,0x0E,0x0F,0x00,0x00,0x00,
0x00,0x9C,0x0E,0x8F,0x28,0x01,0x96,0xB9,0xA3,0xFF,
/*  GRAPHICS regs */
0x00,0x00,0x00,0x00,0x00,0x10,0x0c,0x00,0xFF,
/*  ATTRIBUTE CONTROLLER regs */
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x08,0x00
};


unsigned char mode07h[62] = {
/*  MISC reg,  STATUS reg,    SEQ regs */
   0x066,     0x00,          0x03,0x00,0x03,0x00,0x02,
/*  CRTC regs */
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
/*  GRAPHICS regs */
0x00,0x00,0x00,0x00,0x00,0x10,0x0A,0x00,0xFF,
/*  ATTRIBUTE CONTROLLER regs */
0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x0E,0x00,0x0F,0x00,0x00
};

#endif