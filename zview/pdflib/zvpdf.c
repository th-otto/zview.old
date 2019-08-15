/*
 * zvpdf.c - initialization code for the shared library
 *
 * Copyright (C) 2019 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright file.
 */

#include <stdio.h>
#include <mint/basepage.h>
#include <mint/mintbind.h>
#include <mint/slb.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <slb/freetype.h>
#include "../zview.h"
#include "../general.h"
#include "../winimg.h"
#include "../plugins/common/imginfo.h"
#include "../plugins/common/zvplugin.h"
#include "../plugins/common/plugin.h"
#include "../plugins/common/plugin_version.h"
#include "zvpdf.h"
#include "pdflib.h"

#if defined(__MSHORT__) || defined(__PUREC__) || defined(__AHCC__)
# error "the slb must not be compiled with -mshort"
#endif

extern char const slb_header[];
static const BASEPAGE *my_base;

/*
 * referenced from header.S
 */
long slb_init(void);
void slb_exit(void);
long slb_open(BASEPAGE *bp);
void slb_close(BASEPAGE *bp);
long __CDECL slb_control(long fn, void *arg);


static struct _zvpdf_funcs *my_funcs;

static struct _zvpdf_funcs *get_slb_funcs(void)
{
	return my_funcs;
}


/*
 * these are not optional and cannot be set
 * to zero in the header, even if they
 * currently don't do anything
 */
long slb_init(void)
{
	const BASEPAGE *bp;
	const long *exec_longs;

	bp = (BASEPAGE *)(slb_header - 256);
	exec_longs = (const long *)((const char *)bp + 28);
	if ((exec_longs[0] == 0x283a001aL && exec_longs[1] == 0x4efb48faL) ||
		(exec_longs[0] == 0x203a001aL && exec_longs[1] == 0x4efb08faL))
		bp = (const BASEPAGE *)((const char *)bp - 228);
	my_base = bp;
	return 0;
}


void slb_exit(void)
{
}


long slb_open(BASEPAGE *bp)
{
	/*
	 * check if SLB is already in use by this process;
	 * should not happen since MiNT should have taken care of that already
	 */
	if (my_funcs != NULL)
		return -EACCES;
	
	return 0;
}


void slb_close(BASEPAGE *bp)
{
	my_funcs = NULL;
}


/*
 * get the function table pointer passed from the application.
 * Automatically done in slb_zlib_open()
 */
__attribute__((__noinline__))
static long set_imports(struct _zvpdf_funcs *funcs)
{
	if (funcs->struct_size != sizeof(*funcs))
		return -EINVAL;
	if (funcs->plugin_version > PLUGIN_VERSION)
		return -EBADARG;
	if (funcs->int_size != sizeof(int))
		return -ERANGE;
	my_funcs = funcs;

	p_get_text_width = funcs->p_get_text_width;

	return 0;
}


__attribute__((__noinline__))
static long slb_compile_flags(void)
{
	long flags = 0;
#if defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
    flags |= (1L << 16);
#endif
#if defined(__mcoldfire__)
    flags |= (1L << 17);
#endif
	return flags;
}


long __CDECL slb_control(long fn, void *arg)
{
	switch ((int)fn)
	{
	case 0:
		return slb_compile_flags();
	case 1:
		return set_imports(arg);
	case 2:
		return (long)my_base;
	case 3:
		return (long)slb_header;
	case 4:
		return (long)my_base->p_cmdlin;
	case 5:
		return (long)(FREETYPE_SHAREDLIB_NAME "\0");
	}
	return -ENOSYS;
}


long zvpdf_freetype_open(void)
{
	return get_slb_funcs()->p_slb_open(LIB_FREETYPE);
}


void zvpdf_freetype_close(void)
{
	get_slb_funcs()->p_slb_close(LIB_FREETYPE);
}


SLB *slb_freetype_get(void)
{
	return get_slb_funcs()->p_slb_get(LIB_FREETYPE);
}
