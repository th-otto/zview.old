/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * ldg_init() : the LDG interface from LDG-library side
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
 * $Id: ldglib.c 130 2015-08-27 21:41:27Z landemarre $
 */

#include <stdlib.h>
#include <string.h>

#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#else
#include <osbind.h>
#endif

#include "version.h"
#include "options.h"
#include "ldg.h"

#ifdef __SOZOBONX__
volatile static LDG lib;
#else
static LDG lib;
#endif

/*
 * Diversion of functions to make them compatible "stdcall"
 * ie, only a0-a1-d1 d0 registers are scratch registers.
 * other registers (such as a2 and d2) must be preserved,
 * which is not the case with pure sozobon and c.
 * (add Arnaud Bercegeay November 2001)
 */

/* Protection contexts (registers), also used by ldg_callback () */

#define NB_REG_CONTEXT 512
unsigned long _ldg_nb_regctx = NB_REG_CONTEXT;

struct
{
	unsigned long _r1, _r2, _r3, _r4;
} _ldg_adr_regctx[NB_REG_CONTEXT];


#ifndef __GNUC__

/* new destination diverted functions */
/* See stdcall.s */
extern void _ldg_begin_stdcall(void);

/* entry point redirection functions */

#define NB_PROC_REDIRECT 500
static struct
{
	short _r1;
	void *_r2;
	short _r3;
	void *_r4;
} tab_redirect_proc[NB_PROC_REDIRECT];


static void trnfm_to_stdcall(void)
{
	int i;

	if (lib.num > NB_PROC_REDIRECT)
	{
		Cconws("\r\nldg_trnfm_to_stdcall failed (tab_redirect_proc too small)\r\n");
		return;
	}

	/* init procedures redirection */
	for (i = 0; i < lib.num; i++)
	{
		tab_redirect_proc[i]._r1 = 0x41F9;	            /* LEA A0, <abs.l ea> */
		tab_redirect_proc[i]._r2 = lib.list[i].func;	/* the ea */
		tab_redirect_proc[i]._r3 = 0x4EF9;	            /* JMP <abs.l ea> */
		tab_redirect_proc[i]._r4 = _ldg_begin_stdcall;
		lib.list[i].func = &tab_redirect_proc[i];
	}

	lib.flags |= LDG_STDCALL;
#ifndef __mcoldfire__
	ldg_cpush();
#endif
}
#endif /* __GNUC__ */

/*
 *	Interface LDG des libraries
 */

int ldg_init(const LDGLIB *liblib)
{
	char *env;
	long *offset_pere;
	long offset = (long) &lib;			/* directly */

	lib.magic = LDG_COOKIE;
	lib.vers = liblib->vers;
	lib.num = liblib->num;
	lib.list = liblib->list;
	lib.infos = liblib->infos;
	lib.flags = liblib->flags;
	lib.close = liblib->close;
	lib.vers_ldg = LDG_NUM;
	lib.user_ext = liblib->user_ext;
	lib.addr_ext = 0L;

	/* IMPORTANT: the buffer HAVE TO be zero-ed before use */
	/* memset( _ldg_adr_regctx, 0, sizeof(_ldg_adr_regctx));  */
	{
		short len = (short)sizeof(_ldg_adr_regctx);
		short i;
		unsigned char *pt = (unsigned char *) _ldg_adr_regctx;

		for (i = 0; i < len; i++)
		{
			*pt++ = 0;
		}
	}

	/* diversion function to preserve the registers */
	/* a2 and d2 if necessary (sozobon pure and c) */

#ifndef __GNUC__
	lib.flags &= ~LDG_STDCALL;
	trnfm_to_stdcall();
#else
	lib.flags |= LDG_STDCALL;
#endif

	env = getenv("OFFSETLDG");
	if (env)
	{
		/* recovery of the offset address of the client to fill */
		offset_pere = (long *) atol(env);
		*offset_pere = offset;
		return 0;						/* No errors */
	}
	return -1;							/* can not be launched from the desktop */
}


/*
 * True path of the library. Must be called from a function
 * of the library but never from the main () of the lib.
 */
const char *ldg_getpath(void)
{
	return lib.path;
}
