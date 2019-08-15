/*
 * bzip2struct.h - internal header file.
 * List of functions that are exported from
 * the application to bzip2
 *
 * Copyright (C) 2019 Thorsten Otto
 */

#ifndef __BZIP2STRUCT_H__
#define __BZIP2STRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>

struct _bzip2_funcs {
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
	 * version of bzip2 the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long bzip2_vernum;

	FILE *stderr_location;
	FILE *stdout_location;
	FILE *stdin_location;
	
	void *__CDECL (*p_memset)(void *, bz_int_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	bz_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	
	size_t __CDECL (*p_strlen)(const char *);
	char *__CDECL (*p_strcpy)(char *, const char *);
	char *__CDECL (*p_strcat)(char *, const char *);
	bz_int_t __CDECL (*p_strcmp)(const char *, const char *);

	void *__CDECL (*p_malloc)(size_t);
	void __CDECL (*p_free)(void *);

	FILE *__CDECL (*p_fopen)(const char *pathname, const char *mode);
	FILE *__CDECL (*p_fdopen)(int fd, const char *mode);
	bz_int_t __CDECL (*p_fclose)(FILE *stream);
	bz_int_t __CDECL (*p_fflush)(FILE *stream);
	bz_int_t __CDECL (*p_feof)(FILE *stream);
	size_t __CDECL (*p_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	size_t __CDECL (*p_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	bz_int_t __CDECL (*p_sprintf)(char *, const char *, ...);
	bz_int_t __CDECL (*p_printf)(const char *, ...);
	bz_int_t __CDECL (*p_fprintf)(FILE *, const char *, ...);
	
	/* room for later extensions */
	void *unused[39];
};

long __CDECL bzip2_slb_control(long fn, void *arg);
struct _bzip2_funcs *bzip2_get_slb_funcs(void);

#endif
