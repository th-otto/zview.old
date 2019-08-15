/*
 * lib_imp_open.c - loader for library
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * For conditions of distribution and use, see the accompanying README file.
 */
#include "slb/jpeg.h"
#include <stdio.h>
#include <stddef.h>
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
#include "jpeg_struct.h"
#include "../common/slbload.h"
#include "symbols.h"



static struct _jpeglibslb_funcs jpeglibslb_funcs;


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

static void *__CDECL gcc_memset(void *s, INT32 val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static char *__CDECL gcc_strncpy(char *dest, const char *src, size_t n)
{
	return strncpy(dest, src, n);
}

static void *__CDECL gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL gcc_free(void *s)
{
	free(s);
}

static void __CDECL gcc_exit(jint_t e)
{
	exit((int)e);
}

static INT32 __CDECL gcc_fprintf(FILE *fp, const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vfprintf(fp, format, args);
	va_end(args);
	return ret;
}

static jint_t __CDECL gcc_sprintf(char *buf, const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vsprintf(buf, format, args);
	va_end(args);
	return ret;
}

static size_t __CDECL gcc_fread(void *buf, size_t size, size_t count, FILE *fp)
{
	return fread(buf, size, count, fp);
}

static size_t __CDECL gcc_fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
	return fwrite(buf, size, count, fp);
}

static INT32 __CDECL gcc_fflush(FILE *fp)
{
	return fflush(fp);
}

static INT32 __CDECL gcc_ferror(FILE *fp)
{
	return ferror(fp);
}

#define S(x) jpeglibslb_funcs.p_##x = gcc_##x

#else

#define S(x) jpeglibslb_funcs.p_##x = x

#endif


long slb_jpeglib_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *jpeglib = slb_jpeglib_get();
	
	if (!jpeglib)
		return -EBADF;
	if (jpeglib->handle)
		return 0;

	jpeglibslb_funcs.struct_size = sizeof(jpeglibslb_funcs);
	jpeglibslb_funcs.int_size = sizeof(INT32);
	jpeglibslb_funcs.jpeglib_vernum = JPEG_LIB_VERSION;
	jpeglibslb_funcs.stderr_location = stderr;

	S(memset);
	S(memcpy);
	S(strncpy);

	S(malloc);
	S(free);
	S(exit);

	S(fprintf);
	S(sprintf);
	S(fread);
	S(fwrite);
	S(fflush);
	S(ferror);

#undef S
	
	ret = slb_load(JPEG_SHAREDLIB_NAME, slbpath, JPEG_LIB_VERSION, &jpeglib->handle, &jpeglib->exec);
	if (ret < 0)
	{
		slb_jpeglib_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = jpeglib_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_jpeglib_close();
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & (1L << 17)) || cpu > 0)
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & (1L << 17))
#endif
	{
		slb_jpeglib_close();
		return -EINVAL;
	}
	
	ret = jpeglib_slb_control(1, &jpeglibslb_funcs);
	if (ret < 0)
	{
		slb_jpeglib_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(jpeglib_slb_control, slb_jpeglib_get)
