/*
 * The header for MiNT/MagiC shared libraries
 *
 * Copyright (C) 2018 Thorsten Otto
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include "symbols.h"
#include "plugver.h"

	.globl C_SYMBOL_NAME(slb_header)

	.text

#define WITH_FUNCNAMES 1

C_SYMBOL_NAME(slb_header):
	.dc.l	0x70004afc	                    /* magic value (clr.l d0, illegal) */
	.dc.l	_slbname	                    /* pointer to library's (file)name */
	.dc.l	PLUGIN_INTERFACE_VERSION		/* version number */
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
#define FETCH_NARGS_2 movem.l 16(%a7),%d0-%d1
#define MOVE_NARGS_2  movem.l %d0-%d1,4(%a7)
#define FETCH_NARGS_1 move.l 16(%a7),%d0
#define MOVE_NARGS_1  move.l %d0,4(%a7)

#define LIBFUNC(fn, name, nargs) \
_wrap_ ## name : \
	FETCH_NARGS_ ## nargs;  /* get 1st&2nd argument */ \
	MOVE_NARGS_ ## nargs;   /* move them down */ \
	jbra C_SYMBOL_NAME(name)  /* go for it */
#define NOFUNC
#include "exports.h"

_slbname:	.ascii	SHAREDLIB
	.dc.b 0
	.even
