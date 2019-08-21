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
 * @TODO : get a better version of get_cookie (from WinDom or Ssytem()'s Mint)
 */

static long cookieptr(void)
{
	return *((long *) 0x5a0);
}


int ldg_cookie(long cookie, long *value)
{
	long *cookiejar = (long *) Supexec(cookieptr);

	if (cookiejar)
	{
		while (*cookiejar)
		{
			if (*cookiejar == cookie)
			{
				if (value)
					*value = *++cookiejar;
				return (1);
			}
			cookiejar += 2;
		}
	}
	return 0;
}
