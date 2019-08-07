/*
 * zview_plugin_struct.h - internal header file.
 * List of functions that are imported from the application
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef __ZVIEW_PLUGIN_STRUCT_H__
#define __ZVIEW_PLUGIN_STRUCT_H__ 1

#include <stdarg.h>
#include <mint/slb.h>
#include <unistd.h>
#include <setjmp.h>
#include <time.h>
#include <sys/stat.h>
#include "imginfo.h"

#ifndef _CDECL
#define _CDECL
#endif

#define LIB_PNG   0
#define LIB_Z     1
#define LIB_JPEG  2
#define LIB_TIFF  3
#define LIB_LZMA  4
#define LIB_EXIF  5

struct _zview_plugin_funcs {
	/*
	 * sizeof(this struct), as
	 * used by the application.
	 */
	size_t struct_size;
	/*
	 * sizeof of an int of the caller.
	 * As for now, must match the one which
	 * was used to compile the library (32 bit)
	 */
	size_t int_size;
	/*
	 * version of zview.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long plugin_version;

	long _CDECL (*p_slb_open)(zv_int_t lib);
	void _CDECL (*p_slb_close)(zv_int_t lib);
	SLB *_CDECL (*p_slb_get)(zv_int_t lib);
	
	void *_CDECL (*p_memset)(void *, zv_int_t, size_t);
	void *_CDECL (*p_memcpy)(void *, const void *, size_t);
	void *_CDECL (*p_memchr)(const void *, zv_int_t, size_t);
	zv_int_t _CDECL (*p_memcmp)(const void *, const void *, size_t);

	size_t _CDECL (*p_strlen)(const char *);
	char *_CDECL (*p_strcpy)(char *, const char *);
	char *_CDECL (*p_strncpy)(char *, const char *, size_t);
	char *_CDECL (*p_strcat)(char *, const char *);
	char *_CDECL (*p_strncat)(char *, const char *, size_t);
	zv_int_t _CDECL (*p_strcmp)(const char *, const char *);
	zv_int_t _CDECL (*p_strncmp)(const char *, const char *, size_t);

	void *_CDECL (*p_malloc)(size_t);
	void *_CDECL (*p_calloc)(size_t, size_t);
	void *_CDECL (*p_realloc)(void *ptr, size_t size);
	void _CDECL (*p_free)(void *);

	zv_int_t _CDECL (*p_get_errno)(void);
	char *_CDECL (*p_strerror)(zv_int_t);
	__attribute__((__noreturn__)) void _CDECL (*p_abort)(void);
	FILE *stderr_location;

	zv_int_t _CDECL (*p_remove)(const char *);

	zv_int_t _CDECL (*p_open)(const char *, zv_int_t, ...);
	zv_int_t _CDECL (*p_close)(zv_int_t);
	ssize_t _CDECL (*p_read)(zv_int_t, void *, size_t);
	ssize_t _CDECL (*p_write)(zv_int_t, const void *, size_t);
	off_t _CDECL (*p_lseek)(zv_int_t, off_t, zv_int_t);

	FILE *_CDECL (*p_fopen)(const char *, const char *);
	FILE *_CDECL (*p_fdopen)(zv_int_t, const char *);
	zv_int_t _CDECL (*p_fclose)(FILE *);
	zv_int_t _CDECL (*p_fseek)(FILE *, long, zv_int_t);
	zv_int_t _CDECL (*p_fseeko)(FILE *, off_t, zv_int_t);
	long _CDECL (*p_ftell)(FILE *);
	off_t _CDECL (*p_ftello)(FILE *);
	zv_int_t _CDECL (*p_printf)(const char *, ...);
	zv_int_t _CDECL (*p_sprintf)(char *, const char *, ...);
	zv_int_t _CDECL (*p_vsnprintf)(char *, size_t, const char *, va_list);
	zv_int_t _CDECL (*p_vfprintf)(FILE *, const char *, va_list);
	size_t _CDECL (*p_fread)(void *, size_t, size_t, FILE *);
	size_t _CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	zv_int_t _CDECL (*p_ferror)(FILE *);
	zv_int_t _CDECL (*p_fflush)(FILE *);

	zv_int_t _CDECL (*p_rand)(void);
	void _CDECL (*p_srand)(zv_uint_t seed);

	void _CDECL (*p_qsort)(void *base, size_t nmemb, size_t size, zv_int_t (*compar)(const void *, const void *));
	void *_CDECL (*p_bsearch)(const void *key, const void *base, size_t nmemb, size_t size, zv_int_t (*compar)(const void *, const void *));
                     
	time_t _CDECL (*p_time)(time_t *tloc);
	struct tm *_CDECL (*p_localtime)(const time_t *timep);
	struct tm *_CDECL (*p_gmtime)(const time_t *);

	zv_int_t _CDECL (*p_fstat)(zv_int_t fd, struct stat *s);
	
	zv_int_t _CDECL (*p_sigsetjmp)(jmp_buf, zv_int_t);
	__attribute__((__noreturn__)) void _CDECL (*p_longjmp)(jmp_buf, zv_int_t);
	
	double _CDECL (*p_atof)(const char *);

	/* room for later extensions */
	void *unused[31];
};

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef PLUGIN_SLB

#undef memset
#undef memcpy
#undef memchr
#undef memcmp

#undef strlen
#undef strcpy
#undef strncpy
#undef strcat
#undef strncat
#undef strcmp
#undef strncmp

#undef malloc
#undef calloc
#undef realloc
#undef free

#undef errno
#undef strerror
#undef abort
#undef stderr

#undef remove

#undef open
#undef close
#undef read
#undef write
#undef lseek

#undef fopen
#undef fdopen
#undef fclose
#undef fseek
#undef fseeko
#undef ftell
#undef ftello
#undef printf
#undef sprintf
#undef vsnprintf
#undef vfprintf
#undef fread
#undef fwrite
#undef ferror
#undef fflush

#undef rand
#undef srand

#undef qsort
#undef bsearch

#undef time
#undef localtime
#undef gmtime

#undef fstat

#undef sigsetjmp
#undef longjmp

#undef atof

struct _zview_plugin_funcs *get_slb_funcs(void);

#define memset(d, c, l) get_slb_funcs()->p_memset(d, c, l)
#define memcpy(d, s, l) get_slb_funcs()->p_memcpy(d, s, l)
#define memchr(d, c, l) get_slb_funcs()->p_memchr(d, c, l)
#define memcmp(d, s, l) get_slb_funcs()->p_memcmp(d, s, l)

#define strlen(s) get_slb_funcs()->p_strlen(s)
#define strcpy get_slb_funcs()->p_strcpy
#define strncpy get_slb_funcs()->p_strncpy
#define strcat get_slb_funcs()->p_strcat
#define strncat get_slb_funcs()->p_strncat
#define strcmp get_slb_funcs()->p_strcmp
#define strncmp get_slb_funcs()->p_strncmp

#define malloc(s) get_slb_funcs()->p_malloc(s)
#define calloc(n, s) get_slb_funcs()->p_calloc(n, s)
#define realloc(p, s) get_slb_funcs()->p_realloc(p, s)
#define free(p) get_slb_funcs()->p_free(p)

#define errno (get_slb_funcs()->p_get_errno())
#define strerror get_slb_funcs()->p_strerror
#define abort get_slb_funcs()->p_abort
#define stderr (get_slb_funcs()->stderr_location)

#define remove get_slb_funcs()->p_remove

#define open get_slb_funcs()->p_open
#define close get_slb_funcs()->p_close
#define read get_slb_funcs()->p_read
#define write get_slb_funcs()->p_write
#define lseek get_slb_funcs()->p_lseek

#define fopen get_slb_funcs()->p_fopen
#define fdopen get_slb_funcs()->p_fdopen
#define fclose get_slb_funcs()->p_fclose
#define fseek get_slb_funcs()->p_fseek
#define fseeko get_slb_funcs()->p_fseeko
#define ftell get_slb_funcs()->p_ftell
#define ftello get_slb_funcs()->p_ftello
#define fread get_slb_funcs()->p_fread
#define fwrite get_slb_funcs()->p_fwrite
#define ferror get_slb_funcs()->p_ferror
#define fflush get_slb_funcs()->p_fflush

#define printf get_slb_funcs()->p_printf
#define sprintf get_slb_funcs()->p_sprintf
#define vsnprintf get_slb_funcs()->p_vsnprintf
#define vfprintf get_slb_funcs()->p_vfprintf

#if 0
#undef vsnprintf
#define vsnprintf my_vsnprintf
int vsnprintf(char *str, size_t size, const char *fmt, va_list va);
void nf_debugprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif

#define rand get_slb_funcs()->p_rand
#define srand get_slb_funcs()->p_srand

#define qsort get_slb_funcs()->p_qsort
#define bsearch get_slb_funcs()->p_bsearch

#define time get_slb_funcs()->p_time
#define localtime get_slb_funcs()->p_localtime
#define gmtime get_slb_funcs()->p_gmtime

#define fstat(fd, s) get_slb_funcs()->p_fstat(fd, s)

#define sigsetjmp get_slb_funcs()->p_sigsetjmp
#define longjmp get_slb_funcs()->p_longjmp

#define atof(x) get_slb_funcs()->p_atof(x)

#endif /* PLUGIN_SLB */

#endif
