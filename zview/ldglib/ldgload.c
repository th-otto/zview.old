/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2010 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Low-level functions to load and unload a LDG-library
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
 * $Id: ldgload.c 131 2015-08-27 21:42:48Z landemarre $
 */

 /*   13 fev 2010 : Patch from Mikro  : Fix memory protection issue + some clean up       */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#ifndef BASEPAGE
#define	BASEPAGE BASPAG
#endif
#endif
#if defined(__GNUC__) || defined(__SOZOBONX__)
# if (__GNUC__ > 2) || ( __GNUC_MINOR__ > 8 )
#  include <mint/basepage.h>
# else
#  include <basepage.h>
# endif
# include <osbind.h>
# include <mintbind.h>
#endif

#include "options.h"
#include "ldg.h"
#include "global.h"

/*
 * Load a module and returns the address
 * of the LDG structure of the module.
 * Apid: id launcher.
 */
LDG *ldg_load(int apid, const char *path)
{
	LDG *ldg = NULL;
	BASEPAGE *module = NULL;
	char *offsetxt;
	char smallbloc[24];
	char *mbloc = NULL;
	char *ldg_global_env = NULL;
	long size_env;

	/* 
	 * The address of the descriptor of the lib (LDG)
	 * Is provided by the library (can be a
	 * Program memory protected) and is transmitted
	 * Between lib and client via an environment variable!
	 */

	/* to recover the environment of the calling program create a bogus basepage */
	module = (BASEPAGE *) Pexec(5, NULL, "", NULL);
	if (module)
	{
		ldg_global_env = module->p_env;	/* recuperation de l'environnement */

		if (ldg_global_env)
		{
			char *env;

			size_env = 0L;
			env = ldg_global_env;
			while (*env)
			{
				while (*env)
				{
					size_env++;
					env++;
				}
				env++;
				size_env++;
			}
			if (size_env)
			{
				mbloc = (char *) Malloc(size_env + 24);
			}
		}
	}

	if (mbloc)
	{
		offsetxt = mbloc;
	} else
	{
		offsetxt = smallbloc;
	}
#ifdef __GNUC__
	sprintf(offsetxt, "OFFSETLDG=%ld%c%c", (long) &ldg, '\0', '\0');
#else
	strcpy(offsetxt, "OFFSETLDG=");
	ltoa((long) &ldg, offsetxt + 10, 10);
	offsetxt[strlen(offsetxt) + 1] = '\0';
#endif
	if (mbloc)
	{									/* copy the environment */
		char *pt, *env;

		pt = offsetxt;
		env = ldg_global_env;
		while (*pt)
			pt++;
		pt++;
		while (*env)
		{
			while (*env)
			{
				*pt++ = *env++;
			}
			*pt++ = *env++;
		}
		*pt = 0;
	}
	if (module)
	{
		Mfree(module->p_env);
		Mfree(module);
		module = NULL;
	}
	
	/* Load, don't go */
	module = (BASEPAGE *) Pexec(3, (char *) path, NULL, offsetxt);
	if (mbloc)
		Mfree(mbloc);
	if ((long) module < 0L)
	{
		ldg_set_error(LDG_ERR_EXEC);
		return NULL;					/* echec de Pexec */
	}

	/* Empty the cache */
#ifndef __mcoldfire__
	/* Why is this necessary? Pexec(3) should flush the caches internally. */
	/* Anyway, this is never necessary on modern ColdFire operating systems. */
	ldg_cpush();
#endif

	/* Just go */
	(void) Pexec(4, NULL, module, NULL);

	/* Empty the cache */
#ifndef __mcoldfire__
	/* Why is this necessary? The caches were flushed before Pexec(4). */
	ldg_cpush();
#endif
	Pwait();

	if (ldg == NULL ||					/* Not a library */
		ldg->magic != LDG_COOKIE)		/* wrong format library */
	{
		ldg_set_error(LDG_BAD_FORMAT);
		Mfree(module->p_env);
		Mfree(module);
		return NULL;
	}

	ldg->id = (short) (apid & 0xFFFF);	/* now contains the id of the launcher */
	ldg->baspag = module;

	/* since version 1.00 */
	if (ldg->vers_ldg >= 0x100)
	{
		if (strchr(path, '\\') == NULL &&
			strchr(path, '/') == NULL)
		{
			char currpath[PATHLEN];

			currpath[0] = Dgetdrv() + 'A';
			currpath[1] = ':';
			currpath[2] = '\\';
			currpath[3] = '\0';
			Dgetpath(currpath + 2, 0);
			strncpy(ldg->path, currpath, PATHLEN - 1);
			strcat(ldg->path, "\\");
			strcat(ldg->path, path);
		} else
		{
			strncpy(ldg->path, path, PATHLEN - 1);
		}
	}
	return ldg;
}


/*
 * Remove a module from Memory
 */
int ldg_unload(int apid, LDG *ldg)
{
	(void) apid;
	if (ldg)
	{
		if (ldg->close)
			(*ldg->close) ();
		Mfree(((BASEPAGE *) ldg->baspag)->p_env);
		Mfree(ldg->baspag);
		/* At this point we can't use 'ldg' structure anymore.
		 * Formally we've released only ldg->baspag but 'ldg' points
		 * to static LDG struct in the module, i.e. it's part of
		 * process TPA which was just released.
		 */
	}
	return 0;
}
