/*
 * LDG plugin.
 * Copyright (c) 2004-2005 Zorro ( zorro270@yahoo.fr)
 *
 * This plugin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This plugin is distributed in the hope that it will be useful,
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
#include <stdint.h>
#ifdef __GNUC__
#include <osbind.h>
#include <mintbind.h>
#include <mint/falcon.h>
#include <mint/cookie.h>
#include <mt_gem.h>
#else
#include <tos.h>
#include <aes.h>
#endif
#include <setjmp.h>
#ifndef PLUGIN_SLB
#include "../../ldglib/ldg.h"
#include "../libshare/libshare.h"
#endif


#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifndef __boolean_defined
#define __boolean_defined 1
#if defined(__PUREC__) || defined(__TURBOC__) || defined(__AHCC__) || defined(__MSHORT__)
/* because it is a return code from plugin functions */
typedef int32_t boolean;
#else
typedef int boolean;
#endif
#endif

#ifndef MIN
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
