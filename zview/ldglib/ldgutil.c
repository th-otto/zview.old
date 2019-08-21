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

struct _ldg_version ldg_version = { LDG_NAME, LDG_NUM };

#define MagX_COOKIE     0x4D616758L		/* 'MagX' */
#define MiNT_COOKIE     0x4D694E54L		/* 'MiNT' */


#if 0
/*
 * Debug function
 * New version if the <format> string is
 * the format of a GEM alert, use form_alert
 */
void ldg_debug(const char *format, ...)
{
	char *path = NULL;
	char debug[255];
	va_list arglist;
	FILE *log;

	/* Temporary, one could also use the cookie
	 * because it would be more flexible
	 */
	shel_envrn(&path, "LDG_DEBUG=");
	if (path == NULL)
		return;

	/* format */
	if (format)
	{
		va_start(arglist, format);
		vsprintf(debug, format, arglist);
		va_end(arglist);
		if (*format == '[')
			form_alert(1, debug);
		else
		{
			log = fopen(path, "a");
			if (log)
				fputs(debug, log);
		}
	} else
	{
		log = fopen(path, "w");
		if (log)
			fclose(log);
	}
}
#endif


void *ldg_Malloc(long size)
{
	void *bloc = 0L;
	static short flag = 0;				/* is 1,-1 ou 0 */

	if (flag == 0)
	{
		unsigned short version;

		version = (unsigned short) Sversion();
		/* Mxalloc () from 0 review GEMDOS 19 */
		/* Flag = ((major> 0) | | (versn.braker.minor> = 0x19)) 1: -1; */
		/* Frankly there is no difference */

		if (((version & 0xff) > 0) || ((version & 0xff00) >= 0x19))
			flag = -1;
		else
			flag = 1;
	}
	if (flag == -1)
	{
		if (ldg_cookie(MagX_COOKIE, NULL) || ldg_cookie(MiNT_COOKIE, NULL))
			/* overall allocation preferentially TT Ram */
			bloc = (void *) Mxalloc(size, 3 + 8 + 32);
		else
			bloc = (void *) Mxalloc(size, 3);
	} else
		bloc = (void *) Malloc(size);
	return bloc;
}


void *ldg_Calloc(long count, long size)
{
	void *bloc;
	char *pt;
	long i;

	size = count * size;
	bloc = ldg_Malloc(size);
	if (bloc != 0L)
	{
		pt = (char *) bloc;
		for (i = 0L; i < size; i++)
			*pt++ = 0;
	}
	return (bloc);
}


void *ldg_Realloc(void *oldblk, long oldsize, long newsize)
{
	void *newblk;

	newblk = ldg_Malloc(newsize);
	if (newblk)
	{
		memcpy(newblk, oldblk, oldsize);
		ldg_Free(oldblk);
		return newblk;
	}
	return NULL;
}


int ldg_Free(void *memory)
{
	return Mfree(memory);
}
