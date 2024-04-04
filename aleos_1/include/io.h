/* AleOS input/output header            *
 * Copyright (c) 2005 Alessandro Presta */

#ifndef _IO_H_
#define _IO_H_

/* text attribute macros */
#define WHITE_TXT 0x07

/* video input functions */
void read_chr(char *);
void read_str(char *);
void read_int(int *);
void read_flt(float *);
void read_dob(double *);

/* video output functions */
void write_chr(char);
void write_str(char *);
void write_int(int);
void write_flt(float);
void write_dob(double);
void clear_screen();
void write_newline();

/* text attribute function */
void text_attr(char);

/* port output function */
void outportb(unsigned int, unsigned char);

/* cursor function */
void update_cursor();

#endif /* _IO_H_ */
