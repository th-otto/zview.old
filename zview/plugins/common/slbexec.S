/*
 * Copyright (c) Draconis Internet Package Project
 * Jens Heitmann <jh_draconis@users.sourceforge.net>
 * 
 * This software is licenced under the GNU Public License.
 * Please see COPYING for further information.
 *
 * This program is free software * you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program * if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA.
 * or have a look to http://www.gnu.org.
 * 
 * Author: Jens Heitmann
 * Version-Date: 2007-07-15
 */ 

#include "symbols.h"

	.globl C_SYMBOL_NAME(__slb_local_exec)
	.globl C_SYMBOL_NAME(_base)

C_SYMBOL_NAME(__slb_local_exec):
	move.l		4(%sp),%a0			/* Header of shared lib */
	
	move.l		8(%sp),%d0			/* Function number */
	cmp.l		4*17(%a0),%d0		/* Max. function number */
	bcc.s		invalidnum
	
	add.l		%d0,%d0				/* *= sizeof (long) */
	add.l		%d0,%d0
	move.l		4*18(%a0,%d0.l),%d0	/* Function pointer */
	beq.s		invalidnum

	move.l		C_SYMBOL_NAME(_base),%a0	
	move.l		%a0,4(%sp)			/* act_pd onto stack, replacing pointer */

	move.l		%d0,%a0
	jmp			(%a0)

invalidnum:
	moveq.l		#-32,%d0
	rts
