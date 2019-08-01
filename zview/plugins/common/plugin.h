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
#include "imginfo.h"

#ifndef _CDECL
#define _CDECL
#endif

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
	
	void *_CDECL (*p_memset)(void *, zv_int_t, size_t);
	void *_CDECL (*p_memcpy)(void *, const void *, size_t);
	void *_CDECL (*p_memchr)(const void *, zv_int_t, size_t);

	size_t _CDECL (*p_strlen)(const char *);
	char *_CDECL (*p_strcpy)(char *, const char *);
	char *_CDECL (*p_strcat)(char *, const char *);
	zv_int_t _CDECL (*p_strcmp)(const char *, const char *);

	void *_CDECL (*p_malloc)(size_t);
	void _CDECL (*p_free)(void *);

	zv_int_t _CDECL (*p_get_errno)(void);
	char *_CDECL (*p_strerror)(zv_int_t);

	zv_int_t _CDECL (*p_open)(const char *, zv_int_t, ...);
	zv_int_t _CDECL (*p_close)(zv_int_t);
	ssize_t _CDECL (*p_read)(zv_int_t, void *, size_t);
	ssize_t _CDECL (*p_write)(zv_int_t, const void *, size_t);
	off_t _CDECL (*p_lseek)(zv_int_t, off_t, zv_int_t);

	FILE *_CDECL (*p_fopen)(const char *, const char *);
	zv_int_t _CDECL (*p_fclose)(FILE *);
	zv_int_t _CDECL (*p_fseek)(FILE *, long, zv_int_t);
	zv_int_t _CDECL (*p_fseeko)(FILE *, __off_t, zv_int_t);
	long _CDECL (*p_ftell)(FILE *);
	off_t _CDECL (*p_ftello)(FILE *);
	zv_int_t _CDECL (*p_sprintf)(char *, const char *, ...);
	zv_int_t _CDECL (*p_vsnprintf)(char *, size_t, const char *, va_list);
	size_t _CDECL (*p_fread)(void *, size_t, size_t, FILE *);
	size_t _CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	zv_int_t _CDECL (*p_ferror)(FILE *);

	zv_int_t _CDECL (*p_rand)(void);
	void _CDECL (*p_srand)(zv_uint_t seed);

	/* room for later extensions */
	void *unused[32];
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

#undef strlen
#undef strcpy
#undef strcat
#undef strcmp

#undef malloc
#undef free

#undef errno
#undef strerror

#undef open
#undef close
#undef read
#undef write
#undef lseek

#undef fopen
#undef fclose
#undef fseek
#undef fseeko
#undef ftell
#undef ftello
#undef sprintf
#undef vsnprintf
#undef fread
#undef fwrite
#undef ferror

#undef rand

struct _zview_plugin_funcs *get_slb_funcs(void);

#define memset(d, c, l) get_slb_funcs()->p_memset(d, c, l)
#define memcpy(d, s, l) get_slb_funcs()->p_memcpy(d, s, l)
#define memchr(d, c, l) get_slb_funcs()->p_memchr(d, c, l)

#define strlen(s) get_slb_funcs()->p_strlen(s)
#define strcpy get_slb_funcs()->p_strcpy
#define strcat get_slb_funcs()->p_strcat
#define strcmp get_slb_funcs()->p_strcmp

#define malloc get_slb_funcs()->p_malloc
#define free get_slb_funcs()->p_free

#undef errno
#define errno (get_slb_funcs()->p_get_errno())
#define strerror get_slb_funcs()->p_strerror

#define open get_slb_funcs()->p_open
#define close get_slb_funcs()->p_close
#define read get_slb_funcs()->p_read
#define write get_slb_funcs()->p_write
#define lseek get_slb_funcs()->p_lseek

/*
 * only referenced by ioapi.c
 */
#define fopen get_slb_funcs()->p_fopen
#define fclose get_slb_funcs()->p_fclose
#define fseek get_slb_funcs()->p_fseek
#define fseeko get_slb_funcs()->p_fseeko
#define ftell get_slb_funcs()->p_ftell
#define ftello get_slb_funcs()->p_ftello
#define fread get_slb_funcs()->p_fread
#define fwrite get_slb_funcs()->p_fwrite
#define ferror get_slb_funcs()->p_ferror

#define sprintf get_slb_funcs()->p_sprintf
#define vsnprintf get_slb_funcs()->p_vsnprintf

#define DEBUG 0
#if DEBUG
#undef vsnprintf
#define vsnprintf my_vsnprintf
int vsnprintf(char *str, size_t size, const char *fmt, va_list va);
void nf_debugprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif

#define rand get_slb_funcs()->p_rand
#define srand get_slb_funcs()->p_srand

#endif /* PLUGIN_SLB */

#endif