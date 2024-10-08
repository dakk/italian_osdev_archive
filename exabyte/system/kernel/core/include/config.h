/* config.h -- Kernel core configuration header */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#ifdef WIN32	// stiamo compilando in ambiente Windows?
#define HAVE_ASM_USCORE
#endif

/** Area modificabile dall'utente: commentare per disabilitare un opzione,
 scommentare per abilitarla */

/* Impostazioni internazionali -- NOTA: scommentare una sola lingua!!! */
//#define	LANG_IT		// Lingua italiana
#define LANG_US	// Lingua inglese

#define IA32

#endif	/* CONFIG_H */
