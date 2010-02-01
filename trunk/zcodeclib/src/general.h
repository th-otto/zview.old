/*
 * zcodeclib.
 * Copyright (c) 2001-2005 Zorro ( zorro270@yahoo.fr)
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
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __PUREC__
#include <tos.h>
#elif defined (__GNUC__)
#include <osbind.h>
#include <mintbind.h>
#include <dirent.h>
#include <unistd.h>
#endif
#include <gemx.h>
#include <ldg.h>
#include <ldg/mem.h>
#include <types2b.h>
#include "mem.h"

#define shared_malloc( s)	_ldg_malloc( s, ldg_global[2]);
#define shared_free( s)		_ldg_free( s, ldg_global[2]);

#ifndef MIN
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif

