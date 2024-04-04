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

/*deve essere assemblata con as e non con nasm*/
.text
.globl	modf

modf:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$16,%esp
	pushl	%ebx
	fnstcw	-4(%ebp)
	fwait
	movw	-4(%ebp),%ax
	orw	$0x0c3f,%ax
	movw	%ax,-8(%ebp)
	fldcw	-8(%ebp)
	fwait
	fldl	8(%ebp)
	frndint
	fstpl	-16(%ebp)
	fwait
	movl	-16(%ebp),%edx
	movl	-12(%ebp),%ecx
	movl	16(%ebp),%ebx
	movl	%edx,(%ebx)
	movl	%ecx,4(%ebx)
	fldl	8(%ebp)
	fsubl	-16(%ebp)
	leal	-20(%ebp),%esp
	fclex
	fldcw	-4(%ebp)
	fwait
	popl	%ebx
	leave
	ret
