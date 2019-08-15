/*
 * impopen.c - loader for library
 *
 * Copyright (C) 2019 Thorsten Otto
 */
#include "slb/lzma.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <mint/cookie.h>
#include <mint/mintbind.h>
#include <math.h>
#include <setjmp.h>
#include <slb/zlib.h>
#include "lzma_struct.h"
#include "../common/slbload.h"
#include "lzma.h"
#include "symbols.h"

static struct _lzma_funcs lzma_funcs;


typedef struct
{
	long key;
	long value;
} COOKIE;


static COOKIE *get_cookie_jar(void)
{
	return (COOKIE *) Setexc(360, (void (*)()) -1L);
}


static int get_cookie(long key, long *value)
{
	COOKIE *cookiejar = get_cookie_jar();

	*value = 0;
	if (cookiejar)
	{
		while (cookiejar->key)
		{
			if (cookiejar->key == key)
			{
				*value = cookiejar->value;
				return 1;
			}
			cookiejar++;
		}
	}
	return 0;
}


#if defined(__MSHORT__) || defined(__PUREC__) || defined(__AHCC__)

static void *__CDECL lzma_gcc_memset(void *s, lzma_int_t val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL lzma_gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static lzma_int_t __CDECL lzma_gcc_memcmp(const void *s, const void *d, size_t len)
{
	return memcmp(s, d, len);
}

static size_t __CDECL lzma_gcc_strlen(const char *s)
{
	return strlen(s);
}

static char *__CDECL lzma_gcc_strcpy(char *d, const char *s)
{
	return strcpy(d, s);
}

static lzma_int_t __CDECL lzma_gcc_strcmp(const char *d, const char *s)
{
	return strcmp(d, s);
}

static void *__CDECL lzma_gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL lzma_gcc_free(void *s)
{
	free(s);
}


#define S(x) lzma_funcs.p_##x = lzma_gcc_##x

#else

#define S(x) lzma_funcs.p_##x = x

#endif


long slb_lzma_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *lzma = slb_lzma_get();
	
	if (!lzma)
		return -EBADF;
	if (lzma->handle)
		return 0;

	lzma_funcs.struct_size = sizeof(lzma_funcs);
	lzma_funcs.int_size = sizeof(lzma_int_t);
	lzma_funcs.lzma_vernum = LZMA_VERSION;

	S(memset);
	S(memcpy);
	S(memcmp);

	S(strlen);
	S(strcpy);
	S(strcmp);

	S(malloc);
	S(free);

#undef S

	ret = slb_load(LZMA_SHAREDLIB_NAME, slbpath, LZMA_VERSION, &lzma->handle, &lzma->exec);
	if (ret < 0)
	{
		slb_lzma_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = lzma_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_lzma_close();
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & (1L << 17)))
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & (1L << 17))
#endif
	{
		slb_lzma_close();
		return -EINVAL;
	}
	
	ret = lzma_slb_control(1, &lzma_funcs);
	if (ret < 0)
	{
		slb_lzma_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(lzma_slb_control, slb_lzma_get)
