/*
 * Copyright (c) Draconis Internet Package Project
 * Jens Heitmann <jh_draconis@users.sourceforge.net>
 * 
 * This software is licenced under the GNU Public License.
 * Please see COPYING for further information.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA.
 * or have a look to http://www.gnu.org.
 * 
 * Author: Jens Heitmann
 * Version-Date: 2007-07-15
 * 
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mint/slb.h>
#include <mint/osbind.h>
#include <mint/mintbind.h>
#include <mint/basepage.h>
#include "slbload.h"


/* just in case; we need the TOS error codes here */
#undef E_OK
#undef EBADARG
#undef ENOSYS
#undef EPLFMT
#define E_OK	  0					/* OK, no error */
#define ENOSYS  -32					/* function not implemented */
#define EBADARG	-64					/* range error */
#define EPLFMT	-66					/* invalid program load format */


/* Prototype of SLBExec function */
#ifndef NO_LOCAL_SLB
long __slb_local_exec(void *slb, ...);
#endif

#ifdef __GNUC__
#define _BasPag _base
#endif

static int os_slb = 1;
static const char *user_slbpath;

typedef struct
{
	long magic;
	const char *name;
	long version;
	long flags;
	void __CDECL (*slb_init) (void);
	void __CDECL (*slb_exit) (void);
	void __CDECL (*slb_open) (BASEPAGE *);
	void __CDECL (*slb_close) (BASEPAGE *);
	const char *const *procnames;
	long next;
	long reserved[7];
	long num_funcs;
	/* long funcs_table[]; */
} SLB_HEADER;

#ifndef NO_LOCAL_SLB
static long localSlbLoad(const char *sharedlib, const char *path, long ver, SLB_HANDLE *slb, SLB_EXEC *slbexec)
{
	long *exec_longs;
	char file[256];
	BASEPAGE *bp;
	SLB_HEADER *slbheader;
	size_t j, len;
	int i;

	if (path)
	{
		len = strlen(path);
		strcpy(file, path);

		for (j = 0; j < len; j++)
			if (file[j] == '/')
				file[j] = '\\';
		if (file[len - 1] != '\\')
			strcat(file, "\\");
	} else
	{
		file[0] = 0;
	}
	
	strcat(file, sharedlib);

	bp = (BASEPAGE *) Pexec(3, file, "\0", 0L);
	for (i = 0; i < 2 && (long) bp < 0; i++)
	{
		const char *ptr;
		const char *slb_path;

		if (i == 0)
		{
			slb_path = user_slbpath;
		} else
		{
			slb_path = getenv("SLBPATH");
			if (slb_path == NULL)
				slb_path = "C:\\mint\\slb";
		}
		
		if (slb_path != NULL)
		{
			while (slb_path[0])
			{
				ptr = strchr(slb_path, ';');
				if (!ptr)
					ptr = slb_path + strlen(slb_path);

				strncpy(file, slb_path, ptr - slb_path);
				file[ptr - slb_path] = 0;
				len = strlen(file);
				while (len && (file[len - 1] == ' ' || file[len - 1] == '\t'))
					file[--len] = 0;

				for (j = 0; j < len; j++)
					if (file[j] == '/')
						file[j] = '\\';
				if (file[len - 1] != '\\')
					strcat(file, "\\");

				strcat(file, sharedlib);
				bp = (BASEPAGE *) Pexec(3, file, "\0", 0L);
				if ((long) bp > 0)
				{
					break;
				}
				slb_path = ptr;
				while (*slb_path == ' ' || *slb_path == '\t' || *slb_path == ';')
					slb_path++;
			}
		}
	}

	if ((long) bp <= 0)
		return (long) bp;

	if (bp->p_env)
	{
		Mfree(bp->p_env);
		bp->p_env = 0;
	}

	/* Test for the new programm-format */
	exec_longs = (long *) ((char *) bp->p_tbase);
	if ((exec_longs[0] == 0x283a001aL && exec_longs[1] == 0x4efb48faL) ||
		(exec_longs[0] == 0x203a001aL && exec_longs[1] == 0x4efb08faL))
	{
		slbheader = (SLB_HEADER *) ((char *) bp->p_tbase + 228L);
	} else
	{
		slbheader = (SLB_HEADER *) (bp->p_tbase);
	}

	if (slbheader->magic != 0x70004afcL)
	{
		Mfree(bp);
		return EPLFMT;
	}

	if (slbheader->version < ver)
	{
		Mfree(bp);
		return EBADARG;
	}

	Mshrink(bp, sizeof(BASEPAGE) + bp->p_tlen + bp->p_dlen + bp->p_blen);
	if (strlen(file) > 127)
		file[127] = 0;
	strcpy(bp->p_cmdlin, file);

	slbheader->slb_init();
	slbheader->slb_open(_BasPag);

	*slb = (SLB_HANDLE) slbheader;
	*slbexec = (SLB_EXEC) __slb_local_exec;
	return E_OK;
}

static long localSlbUnload(SLB_HANDLE slb)
{
	((SLB_HEADER *) slb)->slb_close(_BasPag);
	((SLB_HEADER *) slb)->slb_exit();

	Mfree(slb);
	return E_OK;
}

#endif


long slb_load(const char *sharedlib, const char *path, long ver, SLB_HANDLE *slb, SLB_EXEC *slbexec)
{
	long err;

	err = Slbopen(sharedlib, path, ver, slb, slbexec);
	if (err == ENOSYS || err == EPLFMT)
	{
#ifndef NO_LOCAL_SLB
		os_slb = 0;
		err = localSlbLoad(sharedlib, path, ver, slb, slbexec);
#endif
	} else if (err < 0)
	{
		const char *ptr;
		const char *slb_path;
		size_t l;
		size_t j;
		long err2;
		char file[256];

		slb_path = user_slbpath;

		if (slb_path != NULL)
		{
			while (slb_path[0])
			{
				ptr = strchr(slb_path, ';');
				if (!ptr)
					ptr = slb_path + strlen(slb_path);

				strncpy(file, slb_path, ptr - slb_path);
				file[ptr - slb_path] = 0;
				l = strlen(file);
				while (l && (file[l - 1] == ' ' || file[l - 1] == '\t'))
					file[--l] = 0;

				for (j = 0; j < l; j++)
					if (file[j] == '/')
						file[j] = '\\';
				if (file[l - 1] != '\\')
					strcat(file, "\\");

				err2 = Slbopen(sharedlib, file, ver, slb, slbexec);
				if (err2 > 0)
				{
					err = err2;
					break;
				}
				slb_path = ptr;
				while (*slb_path == ' ' || *slb_path == '\t' || *slb_path == ';')
					slb_path++;
			}
		}
	}

	if (err < 0)
	{
		*slb = 0;
		return err;
	}

	return 0;
}


long slb_unload(SLB_HANDLE slb)
{
	long err;

	if (os_slb)
		err = Slbclose(slb);
	else
		err = localSlbUnload(slb);

	if (err < 0)
		return err;

	return 0;
}


void slb_setpath(const char *slbpath)
{
	user_slbpath = slbpath;
}
