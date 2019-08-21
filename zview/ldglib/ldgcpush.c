/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Some usefull functions (for LDG kernel & client application)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: ldgutil.c 131 2015-08-27 21:42:48Z landemarre $
 * Patch Coldfire Vincent Riviere
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "version.h"
#include "options.h"
#include "ldg.h"
#include "global.h"

/*#include <gem.h>*/
#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#endif
#if defined(__GNUC__) || defined(__SOZOBONX__)
#include <osbind.h>
#define BASPAG	BASEPAGE
#endif


/*
 *  ldg_cpush
 *
 * Empty caches and data processor instruction
 * By calling the asm function "cpush".
 * This code can be called from the 68040 and must be
 * executed in supervisor mode (hence the use of Supexec).
 *
 * 1st version April 17, 2002 by Arnaud Bercegeay <bercegeay@atari.org>
 */

#define CPU_COOKIE 0x5F435055UL			/* _CPU */

void ldg_cpush(void)
{
#ifndef __mcoldfire__
	static long _cpu = -1;

	static unsigned short const do_cpush[] = {
		0x4E71,							/* NOP         ; 1 nop useful for some 68040 */
		0xF4F8,							/* CPUSHA BC   ; emptying caches data & instruction */
		0x4E75							/* RTS         ; end of the procedure */
	};

	if (_cpu < 0)
	{
		ldg_cookie(CPU_COOKIE, &_cpu);
		_cpu &= 0xFFFF;
	}

	if (_cpu > 30)
		Supexec((long (*)(void)) do_cpush);
#endif
}
