
; /******************************************************************************
; * Exabyte Operating System                                                    *
; *                                                                             *
; * This program is free software; you can redistribute it and/or               *
; * modify it under the terms of the GNU General Public License                 *
; * as published by the Free Software Foundation; either version 2              *
; * of the License, or (at your option) any later version.                      *
; *                                                                             *
; * This program is distributed in the hope that it will be useful,             *
; * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
; * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
; * GNU General Public License for more details.                                *
; *                                                                             *
; * You should have received a copy of the GNU General Public License           *
; * along with this program; if not, write to the Free Software                 *
; * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
; *******************************************************************************/

GLOBAL _ia32_spinlock_acquire
GLOBAL ia32_spinlock_acquire

_ia32_spinlock_acquire:
ia32_spinlock_acquire:
	mov	eax,[esp+4]
	cmp	dword [eax],0
	je	_ia32_spinlock_get
	pause
	jmp	_ia32_spinlock_acquire
_ia32_spinlock_get:
	mov	ebx,1
	xchg	ebx,dword [eax]
	cmp	ebx,0
	jne	_ia32_spinlock_acquire
	ret

GLOBAL _ia32_spinlock_release
GLOBAL ia32_spinlock_release

ia32_spinlock_release:
_ia32_spinlock_release:
	mov	eax,[esp+4]
	mov	dword [eax],0
	ret
