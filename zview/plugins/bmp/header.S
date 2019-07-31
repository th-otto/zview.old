/*
 * The header for MiNT/MagiC shared libraries
 *
 * Copyright (C) 2018 Thorsten Otto
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
	.text

#include "symbols.h"

#define WITH_FUNCNAMES 1

	.dc.l	0x70004afc	                    /* magic value (clr.l d0, illegal) */
	.dc.l	_slbname	                    /* pointer to library's (file)name */
	.dc.l	0x0200							/* version number */
	.dc.l	0	                            /* flags, currently 0L */
	.dc.l	C_SYMBOL_NAME(slb_init)			/* called when the library's been loaded */
	.dc.l	C_SYMBOL_NAME(slb_exit)			/* called before the library is removed */
	.dc.l	C_SYMBOL_NAME(slb_open)			/* called when a process opens the library */
	.dc.l	C_SYMBOL_NAME(slb_close)		/* called when a process closes the library */
#if WITH_FUNCNAMES
	.dc.l	_slh_names
#else
	.dc.l	0	                            /* pointer to function names, optional */
#endif
	.dc.l	0                               /* used by MetaDOS loader */
	.dc.l	0,0,0,0,0,0,0	                /* unused, always 0L */

	.dc.l	(slh_fend-slh_functions)/4	    /* number of functions */

slh_functions:
/*
 * create the dispatch table
 */
#define LIBFUNC(fn, name, nargs) .dc.l _wrap_ ## name
#define NOFUNC .dc.l 0
#include "exports.h"
slh_fend:


#if WITH_FUNCNAMES
/*
 * create the function names
 */
#define LIBFUNC(fn, name, nargs) name_##name##_str: .ascii #name; .dc.b 0
#define NOFUNC
#include "exports.h"
	.even

/*
 * create the function name table
 */
_slh_names:
#define LIBFUNC(fn, name, nargs) .dc.l name_##name##_str
#define NOFUNC .dc.l 0
#include "exports.h"

#endif

/*
 * create the entry points
 */
#define LIBFUNC(fn, name, nargs) \
_wrap_ ## name : \
	move.l (%a7)+,%d0; /* get return pc */ \
	move.l (%a7)+,%a0; /* get callers basepage */ \
	addq.l #4,%a7;     /* skip function number */ \
	move.l %d0,(%a7);  /* put return pc, overwriting #arguments */ \
	jbra C_SYMBOL_NAME(name)
#define NOFUNC
#include "exports.h"

_slbname:	.ascii	SHAREDLIB
	.dc.b 0
	.even
