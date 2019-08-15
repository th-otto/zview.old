/*
 * impopen.c - loader for library
 *
 * Copyright (C) 2019 Thorsten Otto
 */
#include "slb/bzip2.h"
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
#include "bzip2_struct.h"
#include "../common/slbload.h"
#include "symbols.h"

#define BZIP2_VERSION 1000006L


static struct _bzip2_funcs bzip2_funcs;


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

static void *__CDECL gcc_memset(void *s, bz_int_t val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static bz_int_t __CDECL gcc_memcmp(const void *s, const void *d, size_t len)
{
	return memcmp(s, d, len);
}

static size_t __CDECL gcc_strlen(const char *s)
{
	return strlen(s);
}

static char *__CDECL gcc_strcpy(char *d, const char *s)
{
	return strcpy(d, s);
}

static char *__CDECL gcc_strcat(char *d, const char *s)
{
	return strcat(d, s);
}

static bz_int_t __CDECL gcc_strcmp(const char *d, const char *s)
{
	return strcmp(d, s);
}

static FILE *__CDECL gcc_fopen(const char *fname, const char *mode)
{
	return fopen(fname, mode);
}

#if defined(__PUREC__) && !defined(_MINTLIB_SOURCE)
FILE *fdopen(int sfd, const char *mode)
{
	FILE *newfd;

	newfd = fopen("CON:", mode);
	if (newfd)
	{
		newfd->Handle = sfd;
		newfd->Flags |= 0x08|0x80; /* _FIOBUF|_FIOBIN */
	}
	return newfd;
}
#endif

static FILE *__CDECL gcc_fdopen(int fd, const char *mode)
{
	return fdopen(fd, mode);
}

static bz_int_t __CDECL gcc_fclose(FILE *fp)
{
	return fclose(fp);
}

static bz_int_t __CDECL gcc_ferror(FILE *fp)
{
	return ferror(fp);
}

static bz_int_t __CDECL gcc_feof(FILE *fp)
{
	return feof(fp);
}

static bz_int_t __CDECL gcc_fflush(FILE *fp)
{
	return fflush(fp);
}

static size_t __CDECL gcc_fread(void *buf, size_t size, size_t count, FILE *fp)
{
	return fread(buf, size, count, fp);
}

static size_t __CDECL gcc_fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
	return fwrite(buf, size, count, fp);
}

static bz_int_t __CDECL gcc_fprintf(FILE *fp, const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vfprintf(fp, format, args);
	va_end(args);
	return ret;
}

static bz_int_t __CDECL gcc_printf(const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vprintf(format, args);
	va_end(args);
	return ret;
}

static bz_int_t __CDECL gcc_sprintf(char *buf, const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vsprintf(buf, format, args);
	va_end(args);
	return ret;
}

static void *__CDECL gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL gcc_free(void *s)
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

#define S(x) bzip2_funcs.p_##x = gcc_##x

#else

#define S(x) bzip2_funcs.p_##x = x

#endif


long slb_bzip2_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *bzip2 = slb_bzip2_get();
	
	if (!bzip2)
		return -EBADF;
	if (bzip2->handle)
		return 0;

	bzip2_funcs.struct_size = sizeof(bzip2_funcs);
	bzip2_funcs.int_size = sizeof(bz_int_t);
	bzip2_funcs.bzip2_vernum = BZIP2_VERSION;
	bzip2_funcs.stderr_location = stderr;
	bzip2_funcs.stdout_location = stdout;
	bzip2_funcs.stdin_location = stdin;

	S(memset);
	S(memcpy);
	S(memcmp);

	S(strlen);
	S(strcpy);
	S(strcat);
	S(strcmp);

	S(malloc);
	S(free);

	S(fopen);
	S(fdopen);
	S(fclose);
	S(fflush);
	S(feof);
	S(fread);
	S(fwrite);
	S(sprintf);
	S(printf);
	S(fprintf);
#undef S

	ret = slb_load(BZIP2_SHAREDLIB_NAME, slbpath, BZIP2_VERSION, &bzip2->handle, &bzip2->exec);
	if (ret < 0)
	{
		slb_bzip2_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = bzip2_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_bzip2_close();
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
		slb_bzip2_close();
		return -EINVAL;
	}
	
	ret = bzip2_slb_control(1, &bzip2_funcs);
	if (ret < 0)
	{
		slb_bzip2_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(bzip2_slb_control, slb_bzip2_get)
