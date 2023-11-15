/*
 * zvpdf.c - initialization code for the shared library
 *
 * Copyright (C) 2019 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright file.
 */

#include <stdio.h>
#include <mint/basepage.h>
#include <mint/mintbind.h>
#include <mint/slb.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <slb/freetype.h>
#include "../zview.h"
#include "../general.h"
#include "../winimg.h"
#include "../plugins/common/imginfo.h"
#include "../plugins/common/zvplugin.h"
#include "../plugins/common/plugin.h"
#include "zvpdf.h"
#include "pdflib.h"

extern char const slb_header[];
static const BASEPAGE *my_base;

/*
 * referenced from header.S
 */
long __CDECL slb_init(void);
void __CDECL slb_exit(void);
long __CDECL slb_open(BASEPAGE *bp);
void __CDECL slb_close(BASEPAGE *bp);
long __CDECL slb_control(long fn, void *arg);


static struct _zvpdf_funcs *my_funcs;

static struct _zvpdf_funcs *get_slb_funcs(void)
{
	return my_funcs;
}


/*
 * these are not optional and cannot be set
 * to zero in the header, even if they
 * currently don't do anything
 */
#pragma GCC diagnostic ignored "-Warray-bounds"
long __CDECL slb_init(void)
{
	const BASEPAGE *bp;
	const long *exec_longs;

	bp = (BASEPAGE *)(slb_header - 256);
	exec_longs = (const long *)((const char *)bp + 28);
	if ((exec_longs[0] == 0x283a001aL && exec_longs[1] == 0x4efb48faL) ||
		(exec_longs[0] == 0x203a001aL && exec_longs[1] == 0x4efb08faL))
		bp = (const BASEPAGE *)((const char *)bp - 228);
	my_base = bp;
	return 0;
}
#pragma GCC diagnostic warning "-Warray-bounds"


void __CDECL slb_exit(void)
{
}


long __CDECL slb_open(BASEPAGE *bp)
{
	/*
	 * check if SLB is already in use by this process;
	 * should not happen since MiNT should have taken care of that already
	 */
	if (my_funcs != NULL)
		return -EACCES;
	
	return 0;
}


void __CDECL slb_close(BASEPAGE *bp)
{
	my_funcs = NULL;
}


FILE *stderr;

/*
 * get the function table pointer passed from the application.
 * Automatically done in slb_zlib_open()
 */
__attribute__((__noinline__))
static long set_imports(struct _zvpdf_funcs *funcs)
{
	if (funcs->struct_size != sizeof(*funcs))
		return -EINVAL;
	if (funcs->interface_version > XPDF_INTERFACE_VERSION)
		return -EBADARG;
	if (funcs->int_size != sizeof(int))
		return -ERANGE;
	my_funcs = funcs;

	p_get_text_width = funcs->p_get_text_width;

	stderr = funcs->stderr_location;

	return 0;
}


__attribute__((__noinline__))
static long slb_compile_flags(void)
{
	long flags = 0;
#if defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
    flags |= SLB_M68020;
#endif
#if defined(__mcoldfire__)
    flags |= SLB_COLDFIRE;
#endif
	return flags;
}


long __CDECL slb_control(long fn, void *arg)
{
	switch ((int)fn)
	{
	case 0:
		return slb_compile_flags();
	case 1:
		return set_imports(arg);
	case 2:
		return (long)my_base;
	case 3:
		return (long)slb_header;
	case 4:
		return (long)my_base->p_cmdlin;
	case 5:
		return (long)(FREETYPE_SHAREDLIB_NAME "\0");
	}
	return -ENOSYS;
}

#undef CHECK
#if 0
#define CHECK(s, r) \
	if (get_slb_funcs()->p_ ## s == 0) \
	{ \
		(void) Cconws(#s " was not set\r\n"); \
		r; \
	}
#else
#define CHECK(s, r)
#endif

long zvpdf_freetype_open(void)
{
	CHECK(slb_open, return -33);
	return get_slb_funcs()->p_slb_open(LIB_FREETYPE);
}


void zvpdf_freetype_close(void)
{
	CHECK(slb_close, return);
	get_slb_funcs()->p_slb_close(LIB_FREETYPE);
}


SLB *slb_freetype_get(void)
{
	CHECK(slb_get, return NULL);
	return get_slb_funcs()->p_slb_get(LIB_FREETYPE);
}


/*
 * resolve references from XPDF library
 */
void *malloc(size_t s)
{
	CHECK(malloc, return NULL);
	return get_slb_funcs()->p_malloc(s);
}

void (free)(void *p)
{
	CHECK(free, return);
	get_slb_funcs()->p_free(p);
}

void *realloc(void *ptr, size_t s)
{
	CHECK(realloc, return NULL);
	return get_slb_funcs()->p_realloc(ptr, s);
}

int memcmp(const void *d, const void *s, size_t n)
{
	CHECK(memcmp, return 0);
	return get_slb_funcs()->p_memcmp(d, s, n);
}

void *memcpy(void *d, const void *s, size_t n)
{
	CHECK(memcpy, return d);
	return get_slb_funcs()->p_memcpy(d, s, n);
}

void *memmove(void *d, const void *s, size_t n)
{
	CHECK(memcmp, return d);
	return get_slb_funcs()->p_memcpy(d, s, n);
}

void *memset(void *d, int c, size_t n)
{
	CHECK(memset, return d);
	return get_slb_funcs()->p_memset(d, c, n);
}

size_t strlen(const char *s)
{
	CHECK(strlen, return 0);
	return get_slb_funcs()->p_strlen(s);
}

int strcmp(const char *d, const char *s)
{
	CHECK(strcmp, return 0);
	return get_slb_funcs()->p_strcmp(d, s);
}

int strncmp(const char *d, const char *s, size_t n)
{
	CHECK(strncmp, return 0);
	return get_slb_funcs()->p_strncmp(d, s, n);
}

char *strcpy(char *d, const char *s)
{
	CHECK(strcpy, return d);
	return get_slb_funcs()->p_strcpy(d, s);
}

char *strncpy(char *d, const char *s, size_t n)
{
	CHECK(strncpy, return d);
	return get_slb_funcs()->p_strncpy(d, s, n);
}

char *strtok(char *str, const char *delim)
{
	CHECK(strtok, return NULL);
	return get_slb_funcs()->p_strtok(str, delim);
}

char *strstr(const char *str, const char *delim)
{
	CHECK(strstr, return NULL);
	return get_slb_funcs()->p_strstr(str, delim);
}

char *strchr(const char *str, int c)
{
	CHECK(strchr, return NULL);
	return get_slb_funcs()->p_strchr(str, c);
}

char *strrchr(const char *str, int c)
{
	CHECK(strrchr, return NULL);
	return get_slb_funcs()->p_strrchr(str, c);
}

int strcasecmp(const char *d, const char *s)
{
	CHECK(strcasecmp, return 0);
	return get_slb_funcs()->p_strcasecmp(d, s);
}

int strncasecmp(const char *d, const char *s, size_t n)
{
	CHECK(strncasecmp, return 0);
	return get_slb_funcs()->p_strncasecmp(d, s, n);
}

size_t strcspn(const char *s, const char *reject)
{
	CHECK(strcspn, return 0);
	return get_slb_funcs()->p_strcspn(s, reject);
}

void abort(void)
{
	Pterm(6 | 0x80);
	__builtin_unreachable();
}

char *getcwd(char *buf, size_t size)
{
	CHECK(getcwd, return NULL);
	return get_slb_funcs()->p_getcwd(buf, size);
}

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	CHECK(qsort, return);
	get_slb_funcs()->p_qsort(base, nmemb, size, compar);
}

void srand(unsigned int seed)
{
	CHECK(srand, return);
	get_slb_funcs()->p_srand(seed);
}

int rand(void)
{
	CHECK(rand, return 0);
	return get_slb_funcs()->p_rand();
}

char *getenv(const char *name)
{
	CHECK(getenv, return NULL);
	return get_slb_funcs()->p_getenv(name);
}

char *tmpnam(char *s)
{
	CHECK(tmpnam, return NULL);
	return get_slb_funcs()->p_tmpnam(s);
}

int mkstemp(char *template)
{
	CHECK(mkstemp, return -1);
	return get_slb_funcs()->p_mkstemp(template);
}

time_t mktime(struct tm *tm)
{
	CHECK(mktime, return 0);
	return get_slb_funcs()->p_mktime(tm);
}

size_t strftime(char *s, size_t max, const char *format, const struct tm *tm)
{
	CHECK(strftime, return 0);
	return get_slb_funcs()->p_strftime(s, max, format, tm);
}

int (isspace)(int c)
{
	CHECK(isspace, return 0);
	return get_slb_funcs()->p_isspace(c);
}

int (isalnum)(int c)
{
	CHECK(isalnum, return 0);
	return get_slb_funcs()->p_isalnum(c);
}

int (isalpha)(int c)
{
	CHECK(isalpha, return 0);
	return get_slb_funcs()->p_isalpha(c);
}

int (isxdigit)(int c)
{
	CHECK(isxdigit, return 0);
	return get_slb_funcs()->p_isxdigit(c);
}

int (isupper)(int c)
{
	CHECK(isupper, return 0);
	return get_slb_funcs()->p_isupper(c);
}

int (islower)(int c)
{
	CHECK(islower, return 0);
	return get_slb_funcs()->p_islower(c);
}

int (toupper)(int c)
{
	CHECK(toupper, return c);
	return get_slb_funcs()->p_toupper(c);
}

int (tolower)(int c)
{
	CHECK(tolower, return c);
	return get_slb_funcs()->p_tolower(c);
}

int sprintf(char *str, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vsprintf, return 0);
	va_start(args, format);
	ret = get_slb_funcs()->p_vsprintf(str, format, args);
	va_end(args);
	return ret;
}

int fprintf(FILE *fp, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vfprintf, return 0);
	va_start(args, format);
	ret = get_slb_funcs()->p_vfprintf(fp, format, args);
	va_end(args);
	return ret;
}

int fflush(FILE *fp)
{
	CHECK(fflush, return 0);
	return get_slb_funcs()->p_fflush(fp);
}

int fputs(const char *s, FILE *fp)
{
	CHECK(fputs, return 0);
	return get_slb_funcs()->p_fputs(s, fp);
}

int fputc(int c, FILE *fp)
{
	CHECK(fputc, return c);
	return get_slb_funcs()->p_fputc(c, fp);
}

FILE *fopen(const char *pathname, const char *mode)
{
	CHECK(fopen, return NULL);
	return get_slb_funcs()->p_fopen(pathname, mode);
}

int fclose(FILE *fp)
{
	CHECK(fclose, return -1);
	return get_slb_funcs()->p_fclose(fp);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	CHECK(fread, return 0);
	return get_slb_funcs()->p_fread(ptr, size, nmemb, stream);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	CHECK(fwrite, return 0);
	return get_slb_funcs()->p_fwrite(ptr, size, nmemb, stream);
}

int fseek(FILE *stream, long offset, int whence)
{
	CHECK(fseek, return 0);
	return get_slb_funcs()->p_fseek(stream, offset, whence);
}

long ftell(FILE *stream)
{
	CHECK(ftell, return 0);
	return get_slb_funcs()->p_ftell(stream);
}

int unlink(const char *pathname)
{
	CHECK(unlink, return -1);
	return get_slb_funcs()->p_unlink(pathname);
}

int sscanf(const char *str, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vsscanf, return 0);
	va_start(args, format);
	ret = get_slb_funcs()->p_vsscanf(str, format, args);
	va_end(args);
	return ret;
}

int atoi(const char *nptr)
{
	CHECK(atoi, return 0);
	return get_slb_funcs()->p_atoi(nptr);
}

double atof(const char *nptr)
{
	CHECK(atof, return 0);
	return get_slb_funcs()->p_atof(nptr);
}

int fgetc(FILE *stream)
{
	CHECK(fgetc, return EOF);
	return get_slb_funcs()->p_fgetc(stream);
}

int ungetc(int c, FILE *stream)
{
	CHECK(ungetc, return c);
	return get_slb_funcs()->p_ungetc(c, stream);
}

FILE *fdopen(int fd, const char *mode)
{
	CHECK(fdopen, return NULL);
	return get_slb_funcs()->p_fdopen(fd, mode);
}

#undef errno
/* only referenced from some math functions */
int errno;

/* newer versions of libstdc++ may have references to strtoul */
unsigned long strtoul(const char *__restrict nptr, char **__restrict endptr, int base)
{
	CHECK(strtoul, return NULL);
	return get_slb_funcs()->p_strtoul(nptr, endptr, base);
}
