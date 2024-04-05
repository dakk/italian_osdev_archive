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

#ifndef KERNEL_MATH_H
#define KERNEL_MATH_H

# define M_El		2.7182818284590452353602874713526625L  /* e */
# define M_LOG2El	1.4426950408889634073599246810018922L  /* log_2 e */
# define M_LOG10El	0.4342944819032518276511289189166051L  /* log_10 e */
# define M_LN2l		0.6931471805599453094172321214581766L  /* log_e 2 */
# define M_LN10l	2.3025850929940456840179914546843642L  /* log_e 10 */
# define M_PIl		3.1415926535897932384626433832795029L  /* pi */
# define M_PI_2l	1.5707963267948966192313216916397514L  /* pi/2 */
# define M_PI_4l	0.7853981633974483096156608458198757L  /* pi/4 */
# define M_1_PIl	0.3183098861837906715377675267450287L  /* 1/pi */
# define M_2_PIl	0.6366197723675813430755350534900574L  /* 2/pi */
# define M_2_SQRTPIl	1.1283791670955125738961589031215452L  /* 2/sqrt(pi) */
# define M_SQRT2l	1.4142135623730950488016887242096981L  /* sqrt(2) */
# define M_SQRT1_2l	0.7071067811865475244008443621048490L  /* 1/sqrt(2) */



unsigned long kfloor(double __x);
unsigned long kceil(double __x);
unsigned long pow(unsigned long x, unsigned long power);
void dec2hex(unsigned long x, char *ret);

//ritorno in radiznti
double sin(double angle);
double cos(double angle);
double sqrt(double number);

#endif
