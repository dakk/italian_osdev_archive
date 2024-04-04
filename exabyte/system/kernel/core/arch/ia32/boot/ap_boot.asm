; ap_boot.asm -- APs entry point
;*****************************************************************************;
; Exabyte Operating System                                                    ;
;                                                                             ;
; This program is free software; you can redistribute it and/or               ;
; modify it under the terms of the GNU General Public License                 ;
; as published by the Free Software Foundation; either version 2              ;
; of the License, or (at your option) any later version.                      ;
;                                                                             ;
; This program is distributed in the hope that it will be useful,             ;
; but WITHOUT ANY WARRANTY; without even the implied warranty of              ;
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               ;
; GNU General Public License for more details.                                ;
;                                                                             ;
; You should have received a copy of the GNU General Public License           ;
; along with this program; if not, write to the Free Software                 ;
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. ;
;*****************************************************************************;

[section .ap_boot]
[bits 16]

SMP_AP_GDT_ADDR			equ	01000h
SMP_AP_IDT_ADDR			equ	02000h
SMP_AP_BOOT_CODE_SIZE		equ	0500h
SMP_AP_STACK			equ	0900h

SMP_AP_DATA_BOOTED		equ	SMP_AP_BOOT_CODE_SIZE
SMP_AP_DATA_ID			equ	SMP_AP_BOOT_CODE_SIZE+1
SMP_AP_DATA_CPU_NUM		equ	SMP_AP_BOOT_CODE_SIZE+2

_ap_boot_entry_point:
	cli
	mov	ax,cs
	mov	ds,ax
	mov	byte [ds:SMP_AP_DATA_BOOTED],1
	mov	ah,9Fh
	mov	bx,0B800h
	mov	es,bx
	mov	bx,cs
	shr	bx,8
	add	bx,bx
	sub	bx,1
_ap_boot_entry_point_loop:
	inc	byte [es:bx]
	jmp	_ap_boot_entry_point_loop
