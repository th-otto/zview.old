/*
 * impopen.c - loader for library
 *
 * Copyright (C) 2019 Thorsten Otto
 */
#include "slb/tiff.h"
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
#include <slb/jpeg.h>
#include <slb/lzma.h>
#include "tiff_struct.h"
#include "../common/slbload.h"
#include "tiff.h"
#include "symbols.h"

#ifndef TIFF_VERSION
#define TIFF_VERSION 4000010
#endif

static struct _tiff_funcs tiff_funcs;



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


static int __CDECL get_errno(void)
{
	return errno;
}

static void __CDECL set_errno(int e)
{
	errno = e;
}

#define S(x) tiff_funcs.p_##x = x


static long __CDECL tiff_zlib_open(const char *slbpath)
{
	SLB *tiff = slb_tiff_get();
	SLB *zlib;
	long ret;

	if (!tiff)
		return -EBADF;
	if (!tiff->handle)
		return -EBADF;
	zlib = slb_zlib_get();
	if (!zlib)
		return -EBADF;
	if (zlib->handle == 0)
	{
		ret = slb_zlib_open(slbpath);
		if (ret != 0)
			return ret;
		tiff_funcs.zlib_opened_here = 1;
	}
	tiff_funcs.zlib = zlib;
	return 0;
}


static long __CDECL tiff_jpeg_open(const char *slbpath)
{
	SLB *tiff = slb_tiff_get();
	SLB *jpeg;
	long ret;

	if (!tiff)
		return -EBADF;
	if (!tiff->handle)
		return -EBADF;
	jpeg = slb_jpeglib_get();
	if (jpeg->handle == 0)
	{
		ret = slb_jpeglib_open(slbpath);
		if (ret != 0)
			return ret;
		tiff_funcs.jpeg_opened_here = 1;
	}
	tiff_funcs.jpeg = jpeg;
	return 0;
}


static long __CDECL tiff_lzma_open(const char *slbpath)
{
	SLB *tiff = slb_tiff_get();
	SLB *lzma;
	long ret;

	if (!tiff)
		return -EBADF;
	if (!tiff->handle)
		return -EBADF;
	lzma = slb_lzma_get();
	if (lzma->handle == 0)
	{
		ret = slb_lzma_open(slbpath);
		if (ret != 0)
			return ret;
		tiff_funcs.lzma_opened_here = 1;
	}
	tiff_funcs.lzma = lzma;
	return 0;
}


long slb_tiff_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *tiff = slb_tiff_get();
	
	if (!tiff)
		return -EBADF;
	if (tiff->handle)
		return 0;

	tiff_funcs.struct_size = sizeof(tiff_funcs);
	tiff_funcs.int_size = sizeof(tiff_int_t);
	tiff_funcs.tiff_vernum = TIFF_VERSION;
	tiff_funcs.stderr_location = stderr;

	S(get_errno);
	S(set_errno);
	S(strerror);

	S(memset);
	S(memcpy);
	S(memcmp);
	S(memchr);

	S(strlen);
	S(strcpy);
	S(strcat);
	S(strcmp);
	S(strncmp);

	S(malloc);
	S(calloc);
	S(realloc);
	S(free);

	S(open);
	S(close);
	S(read);
	S(write);
	S(lseek);

	S(fwrite);
	S(sprintf);
	S(snprintf);
	S(vsnprintf);
	S(fprintf);
	S(vfprintf);
	S(fputc);
	S(fputs);

	S(fstat);

	S(sigsetjmp);
	S(longjmp);

	S(rand);
	S(srand);
	S(qsort);
	S(bsearch);
	S(getenv);

#if 0
	/* functions forwarded to jpeglib */
	S(strncpy);
	S(fread);
	S(fflush);
	S(ferror);
	S(exit);
#endif

#undef S

	tiff_funcs.zlib_opened_here = 0;
	tiff_funcs.zlib = 0;
	tiff_funcs.p_slb_tiff_zlib_open = tiff_zlib_open;

	tiff_funcs.jpeg_opened_here = 0;
	tiff_funcs.p_slb_tiff_jpeg_open = tiff_jpeg_open;

	tiff_funcs.lzma_opened_here = 0;
	tiff_funcs.lzma = 0;
	tiff_funcs.p_slb_tiff_lzma_open = tiff_lzma_open;

	ret = slb_load(TIFF_SHAREDLIB_NAME, slbpath, TIFF_VERSION, &tiff->handle, &tiff->exec);
	if (ret < 0)
	{
		slb_tiff_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = tiff_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_tiff_close();
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
		slb_tiff_close();
		return -EINVAL;
	}
	
	ret = tiff_slb_control(1, &tiff_funcs);
	if (ret < 0)
	{
		slb_tiff_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(tiff_slb_control, slb_tiff_get)
