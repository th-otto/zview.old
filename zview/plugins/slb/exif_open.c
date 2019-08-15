/*
 * impopen.c - loader for library
 *
 * Copyright (C) 2019 Thorsten Otto
 */
#include "slb/exif.h"
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
#include "exif_struct.h"
#include "../common/slbload.h"
#include "symbols.h"

#define EXIF_VERSION 6021


static struct _exif_funcs exif_funcs;


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

static void *__CDECL exif_gcc_memset(void *s, exif_int_t val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL exif_gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static exif_int_t __CDECL exif_gcc_memcmp(const void *s, const void *d, size_t len)
{
	return memcmp(s, d, len);
}

static size_t __CDECL exif_gcc_strlen(const char *s)
{
	return strlen(s);
}

static char *__CDECL exif_gcc_strcpy(char *d, const char *s)
{
	return strcpy(d, s);
}

static exif_int_t __CDECL exif_gcc_strcmp(const char *d, const char *s)
{
	return strcmp(d, s);
}

static void *__CDECL exif_gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL exif_gcc_free(void *s)
{
	free(s);
}

#if defined(__PUREC__) || defined(__AHCC__)
#define ESRCH	3	    /* No such process            */
#define EINTR	4		/* Interrupted system call    */
#define ENXIO	6		/* No such device or address  */
#define EFAULT	14		/* Bad address                */
#define ENOTBLK	15		/* Bulk device required       */
#define EBUSY	16		/* Resource is busy           */
#define ENOTTY	25		/* Not a terminal             */
#define ETXTBSY	26		/* Text file is busy          */
#define EFBIG	27		/* File is too large          */
#define EMLINK	31		/* Too many links             */
#define EPIPE	32		/* Broken pipe                */
#define EILSEQ  84      /* Illegal byte sequence      */
#endif

#define S(x) exif_funcs.p_##x = exif_gcc_##x

#else

#define S(x) exif_funcs.p_##x = x

#endif


static struct _exif_funcs exif_funcs;

long slb_exif_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *exif = slb_exif_get();
	
	if (!exif)
		return -EBADF;
	if (exif->handle)
		return 0;

	exif_funcs.struct_size = sizeof(exif_funcs);
	exif_funcs.int_size = sizeof(exif_int_t);
	exif_funcs.exif_vernum = EXIF_VERSION;

	S(memset);
	S(memcpy);
	S(memcmp);
	S(memchr);

	S(strlen);
	S(strcpy);
	S(strncpy);
	S(strcat);
	S(strncat);
	S(strcmp);
	S(strncmp);

	S(malloc);
	S(calloc);
	S(realloc);
	S(free);

	S(fopen);
	S(fclose);
	S(fread);
	S(sprintf);
	S(snprintf);
	S(printf);

	S(qsort);
	S(bsearch);

	S(time);
	S(localtime);
#undef S

	ret = slb_load(EXIF_SHAREDLIB_NAME, slbpath, EXIF_VERSION, &exif->handle, &exif->exec);
	if (ret < 0)
	{
		slb_exif_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = exif_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_exif_close();
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
		slb_exif_close();
		return -EINVAL;
	}
	
	ret = exif_slb_control(1, &exif_funcs);
	if (ret < 0)
	{
		slb_exif_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(exif_slb_control, slb_exif_get)
