/*
 * libstruct.h - internal header file.
 * List of functions that are exported from
 * the application to jpeg
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * For conditions of distribution and use, see the accompanying README file.
 */

#ifndef __JPEGLIBSTRUCT_H__
#define __JPEGLIBSTRUCT_H__ 1

#include <mint/slb.h>


struct _jpeglibslb_funcs {
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
	 * version of zlib.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long jpeglib_vernum;
	
	FILE *stderr_location;

	void *__CDECL (*p_memset)(void *, jint_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	char *__CDECL (*p_strncpy)(char *, const char *, size_t);
	
	void *__CDECL (*p_malloc)(size_t);
	void __CDECL (*p_free)(void *);
	void __CDECL (*p_exit)(jint_t);

	jint_t __CDECL (*p_fprintf)(FILE *, const char *, ...);
	jint_t __CDECL (*p_sprintf)(char *, const char *, ...);
	size_t __CDECL (*p_fread)(void *, size_t, size_t, FILE *);
	size_t __CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	jint_t __CDECL (*p_fflush)(FILE *);
	jint_t __CDECL (*p_ferror)(FILE *);

	/* room for later extensions */
	void *unused[48];
};

long __CDECL jpeglib_slb_control(long fn, void *arg);

#endif
