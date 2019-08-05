/*
 * pnglib_imp_open.c - loader for library
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 */
#include "slb/png.h"
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
#include "png_struct.h"
#include "../common/slbload.h"
#include "symbols.h"


#undef png_error
#undef png_chunk_error
#undef png_warning
#undef png_chunk_warning
#undef png_benign_error
#undef png_chunk_benign_error

static struct _pnglibslb_funcs pnglibslb_funcs;

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

static void *__CDECL png_gcc_memset(void *s, png_int_t val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL png_gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static png_int_t __CDECL png_gcc_memcmp(const void *s, const void *d, size_t len)
{
	return memcmp(s, d, len);
}

static size_t __CDECL png_gcc_strlen(const char *s)
{
	return strlen(s);
}

static char *__CDECL png_gcc_strcpy(char *d, const char *s)
{
	return strcpy(d, s);
}

static png_int_t __CDECL png_gcc_strcmp(const char *d, const char *s)
{
	return strcmp(d, s);
}

static void *__CDECL png_gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL png_gcc_free(void *s)
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

static png_int_t __CDECL png_gcc_get_errno(void)
{
#if defined(__GNUC__) || defined(_GNU_SOURCE)
	/* we are just here because of -mshort */
	return errno;
#elif defined(__PUREC__) || defined(__AHCC__) || defined(LATTICE)
	/* must translate error numbers from Pure-C library to what mintlib expects */
	switch (errno)
	{
		case EPERM: return 38;
		case ENOENT: return 33;
		case ESRCH: return 20;
		case EINTR: return 128;
		case EIO: return 90;
		case ENXIO: return 46;
		case EBADF: return 37;
#if defined(__PUREC__) || defined(__AHCC__)
		case EILLSPE: return 25; /* -> EINVAL */
		case EINVMEM: return 40; /* -> EFAULT */
#endif
		case EFAULT: return 40;
		case ENOTBLK: return 23;
		case EBUSY: return 2;
		case ENOMEM: return 39;
		case EACCES: return 36;
		case EEXIST: return 85;
#if defined(__PUREC__) || defined(__AHCC__)
		case EPLFMT: return 66; /* -> ENOEXEC */
#endif
#if defined(LATTICE)
		case ENOEXEC: return 66;
		case EAGAIN: return 326;
		case EXDEV: return 48;
		case ECHILD: return 48;
#endif
		case ENODEV: return 15;
		case ENOTDIR: return 34;
		case EINVAL: return 25;
		case ENFILE: return 50;
		case EMFILE: return 35;
		case ENOTTY: return 87;
		case ETXTBSY: return 70;
		case EFBIG: return 71;
		case ENOSPC: return 91;
		case ESPIPE: return 6;
		case EROFS: return 13;
		case EMLINK: return 82;
		case EPIPE: return 81;
		case EDOM: return 89;
		case ERANGE: return 88;
#if defined(__PUREC__) || defined(__AHCC__)
		case ENMFILE: return 49;
#endif
#if defined(LATTICE)
		case EDEADLK: return 22;
#endif
		case EILSEQ: return 27;
	}
	return 1;
#else
 # error "NYI"
	return 1;
#endif
}

static char *__CDECL png_gcc_strerror(png_int_t e)
{
#if defined(__GNUC__) || defined(_GNU_SOURCE)
	return strerror((int)e);
#else
	/* FIXME: would need strerror from mintlib */
	static char errorbuf[14];
	sprintf(errorbuf, "error #%d", (int)e);
	return errorbuf;
#endif
}

static PNG_NORETURN void __CDECL png_gcc_abort(void)
{
	abort();
}

static png_int_t __CDECL png_gcc_remove(const char *fname)
{
	return remove(fname);
}

static FILE *__CDECL png_gcc_fopen(const char *fname, const char *mode)
{
	return fopen(fname, mode);
}

static png_int_t __CDECL png_gcc_fclose(FILE *fp)
{
	return fclose(fp);
}

static png_int_t __CDECL png_gcc_fprintf(FILE *fp, const char *format, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = vfprintf(fp, format, args);
	va_end(args);
	return ret;
}

static size_t __CDECL png_gcc_fread(void *buf, size_t size, size_t count, FILE *fp)
{
	return fread(buf, size, count, fp);
}

static size_t __CDECL png_gcc_fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
	return fwrite(buf, size, count, fp);
}

static png_int_t __CDECL png_gcc_fflush(FILE *fp)
{
	return fflush(fp);
}

static png_int_t __CDECL png_gcc_ferror(FILE *fp)
{
	return ferror(fp);
}

static struct tm *__CDECL png_gcc_gmtime(const time_t *t)
{
	struct tm *tm;
	static struct {
		long tm_sec;			/* Seconds.	[0-60] (1 leap second) */
		long tm_min;			/* Minutes.	[0-59] */
		long tm_hour;			/* Hours.	[0-23] */
		long tm_mday;			/* Day.		[1-31] */
		long tm_mon;			/* Month.	[0-11] */
		long tm_year;			/* Year	- 1900.  */
		long tm_wday;			/* Day of week.	[0-6] */
		long tm_yday;			/* Days in year.[0-365]	*/
		long tm_isdst;			/* DST.		[-1/0/1]*/
		long int tm_gmtoff;		/* Seconds east of UTC.  */
		const char *tm_zone;	/* Timezone abbreviation.  */
	} minttm;
	tm = gmtime(t);
	if (tm == NULL)
		return tm;
	minttm.tm_sec = tm->tm_sec;
	minttm.tm_min = tm->tm_min;
	minttm.tm_hour = tm->tm_hour;
	minttm.tm_mday = tm->tm_mday;
	minttm.tm_mon = tm->tm_mon;
	minttm.tm_year = tm->tm_year;
	minttm.tm_wday = tm->tm_wday;
	minttm.tm_yday = tm->tm_yday;
	minttm.tm_isdst = tm->tm_isdst;
	minttm.tm_gmtoff = timezone;
	return (struct tm *)&minttm;
}

#if defined(__PUREC__) || defined(__AHCC__)

static void *return_in_d0_d1_0(void *) 0x2010;
static void *return_in_d0_d1_1(void *) 0x2228;
static void *return_in_d0_d1_2(void *) 0x0004;

#define return_in_d0_d1(x) return_in_d0_d1_2(return_in_d0_d1_1(return_in_d0_d1_0(&x)))

#pragma warn -stv

/*
 * The functions returning doubles are declared as void here;
 * we cannot return a structure, because
 * - GCC expects doubles to be returned in d0/d1
 * - PureC expects the caller to pass the address of the
 *   structure return value in a hidden argument on the stack
 * - Pure-C has a bug due to this, when that same function
 *   is also declared as cdecl
 */
#if defined(__HAVE_68881__)

static void __CDECL png_gcc_pow(__gcc_double_t x, __gcc_double_t y)
{
	double xx = pc_d64tod96(x);
	double yy = pc_d64tod96(y);
	double vv = pow(xx, yy);
	__gcc_double_t v = pc_d96tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_floor(__gcc_double_t x)
{
	double xx = pc_d64tod96(x);
	double vv = floor(xx);
	__gcc_double_t v = pc_d96tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_frexp(__gcc_double_t x, png_int_t *exp)
{
	double xx = pc_d64tod96(x);
	int e;
	double vv;
	__gcc_double_t v;
	
	vv = frexp(xx, &e);
	if (exp)
		*exp = e;
	v = pc_d96tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_modf(__gcc_double_t x, __gcc_double_t *y)
{
	double xx = pc_d64tod96(x);
	double yy;
	double vv;
	__gcc_double_t v;
	
	vv = modf(xx, &yy);
	if (y)
		*y = pc_d96tod64(yy);
	v = pc_d96tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_atof(const char *s)
{
	double vv = atof(s);
	__gcc_double_t v = pc_d96tod64(vv);
	return_in_d0_d1(v);
}

#define png_gcc_longjmp png_gcc_longjmp881

#else

static void __CDECL png_gcc_pow(__gcc_double_t x, __gcc_double_t y)
{
	double xx = pc_d64tod80(x);
	double yy = pc_d64tod80(y);
	double vv = pow(xx, yy);
	__gcc_double_t v = pc_d80tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_floor(__gcc_double_t x)
{
	double xx = pc_d64tod80(x);
	double vv = floor(xx);
	__gcc_double_t v = pc_d80tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_frexp(__gcc_double_t x, png_int_t *exp)
{
	double xx = pc_d64tod80(x);
	int e;
	double vv;
	__gcc_double_t v;
	
	vv = frexp(xx, &e);
	if (exp)
		*exp = e;
	v = pc_d80tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_modf(__gcc_double_t x, __gcc_double_t *y)
{
	double xx = pc_d64tod80(x);
	double yy;
	double vv;
	__gcc_double_t v;
	
	vv = modf(xx, &yy);
	if (y)
		*y = pc_d80tod64(yy);
	v = pc_d80tod64(vv);
	return_in_d0_d1(v);
}

static void __CDECL png_gcc_atof(const char *s)
{
	double vv = atof(s);
	__gcc_double_t v = pc_d80tod64(vv);
	return_in_d0_d1(v);
}

#endif

#else

static double __CDECL png_gcc_frexp(double x, png_int_t *exp)
{
	int e;
	double v;
	
	v = frexp(x, &e);
	if (exp)
		*exp = e;
	return v;
}

#endif

static png_int_t __CDECL png_gcc_sigsetjmp(jmp_buf buf, png_int_t savesig)
{
	(void)savesig;
	/*
	 * note: mintlib's jmp_buf and Pure-C's have different usages
	 * in the fields, but luckily they are of the same size,
	 * so we can actually do this:
	 */
	return setjmp(buf);
}

/* exported, since it is also used by the png_jmpbuf macro */
void __CDECL png_gcc_longjmp(jmp_buf buf, png_int_t e)
{
	longjmp(buf, (int)e);
}

#define S(x) pnglibslb_funcs.p_##x = png_gcc_##x

#else

static int __CDECL get_errno(void)
{
	return errno;
}

#define S(x) pnglibslb_funcs.p_##x = x

#endif


long slb_pnglib_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *zlib = slb_zlib_get();
	SLB *pnglib = slb_pnglib_get();
	
	if (!pnglib || !zlib)
		return -EBADF;
	if (pnglib->handle)
		return 0;

	pnglibslb_funcs.struct_size = sizeof(pnglibslb_funcs);
	pnglibslb_funcs.int_size = sizeof(png_int_t);
	pnglibslb_funcs.pnglib_vernum = PNG_LIBPNG_VER;
	pnglibslb_funcs.stderr_location = stderr;

	S(memset);
	S(memcpy);
	S(memcmp);

	S(strlen);
	S(strcpy);
	S(strcmp);

	S(malloc);
	S(free);

	S(get_errno);
	S(strerror);
	S(abort);

	S(remove);

	S(fopen);
	S(fclose);
	S(fprintf);
	S(fread);
	S(fwrite);
	S(fflush);
	S(ferror);

	S(gmtime);

	S(pow);
	S(floor);
	S(frexp);
	S(modf);
	S(atof);

	S(sigsetjmp);
	S(longjmp);
#undef S
	
	pnglibslb_funcs.pnglib_zlib_opened_here = 0;
	if (zlib->handle == 0)
	{
		ret = slb_zlib_open(slbpath);
		if (ret != 0)
			return ret;
		pnglibslb_funcs.pnglib_zlib_opened_here = 1;
	}
	pnglibslb_funcs.zlib = zlib;
	
	ret = slb_load(PNG_SHAREDLIB_NAME, slbpath, PNG_LIBPNG_VER, &pnglib->handle, &pnglib->exec);
	if (ret < 0)
	{
		slb_pnglib_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = pnglib_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_pnglib_close();
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
		slb_pnglib_close();
		return -EINVAL;
	}
	
	ret = pnglib_slb_control(1, &pnglibslb_funcs);
	if (ret < 0)
	{
		slb_pnglib_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(pnglib_slb_control, slb_pnglib_get)
