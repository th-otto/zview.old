/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libshare.h>
#include "zutil.h"

#ifndef NO_DUMMY_DECL
struct internal_state      {int dummy;}; /* for buggy compilers */
#endif

#ifndef STDC
extern void exit OF((int));
#endif

const char * const z_errmsg[10] = {
"",     /* Z_NEED_DICT       2  */
"",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"",          /* Z_ERRNO         (-1) */
"",        /* Z_STREAM_ERROR  (-2) */
"",          /* Z_DATA_ERROR    (-3) */
"", /* Z_MEM_ERROR     (-4) */
"",        /* Z_BUF_ERROR     (-5) */
"",/* Z_VERSION_ERROR (-6) */
""};


/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const char * ZEXPORT zError(err)
    int err;
{
    return ERR_MSG(err);
}

voidpf zcalloc (opaque, items, size)
    voidpf opaque;
    unsigned items;
    unsigned size;
{
    if (opaque) items += size - size; /* make compiler happy */
    return sizeof(uInt) > 2 ?	(voidpf)malloc(items * size) : 
								(voidpf)calloc(items, size);
}

void  zcfree (opaque, ptr)
    voidpf opaque;
    voidpf ptr;
{
    free(ptr);
    if (opaque) return; /* make compiler happy */
}

