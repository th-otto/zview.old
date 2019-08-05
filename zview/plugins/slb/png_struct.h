/*
 * pnglibstruct.h - internal header file.
 * List of functions that are exported from
 * the application to pnglib
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 */

#ifndef __PNGLIBSTRUCT_H__
#define __PNGLIBSTRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>


struct _pnglibslb_funcs {
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
	long pnglib_vernum;
	
	png_int_t __CDECL (*p_get_errno)(void);
	FILE *stderr_location;

	SLB *zlib;
	short pnglib_zlib_opened_here;
	short filler;
	
	void *__CDECL (*p_memset)(void *, png_int_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	png_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	
	size_t __CDECL (*p_strlen)(const char *);
	char *__CDECL (*p_strcpy)(char *, const char *);
	png_int_t __CDECL (*p_strcmp)(const char *, const char *);

	void *__CDECL (*p_malloc)(size_t);
	void __CDECL (*p_free)(void *);

	char *__CDECL (*p_strerror)(png_int_t);
	PNG_NORETURN void __CDECL (*p_abort)(void);

	png_int_t __CDECL (*p_remove)(const char *);

	FILE *__CDECL (*p_fopen)(const char *, const char *);
	png_int_t __CDECL (*p_fclose)(FILE *);
	png_int_t __CDECL (*p_fprintf)(FILE *, const char *, ...);
	size_t __CDECL (*p_fread)(void *, size_t, size_t, FILE *);
	size_t __CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	png_int_t __CDECL (*p_fflush)(FILE *);
	png_int_t __CDECL (*p_ferror)(FILE *);

	struct tm *__CDECL (*p_gmtime)(const time_t *);
	
#if defined(__PUREC__) || defined(__AHCC__)
	void __CDECL (*p_pow)(__gcc_double_t, __gcc_double_t);
	void __CDECL (*p_floor)(__gcc_double_t);
	void __CDECL (*p_frexp)(__gcc_double_t, png_int_t *);
	void __CDECL (*p_modf)(__gcc_double_t, __gcc_double_t *);
	void __CDECL (*p_atof)(const char *);
#else
	double __CDECL (*p_pow)(double, double);
	double __CDECL (*p_floor)(double);
	double __CDECL (*p_frexp)(double, png_int_t *);
	double __CDECL (*p_modf)(double, double *);
	double __CDECL (*p_atof)(const char *);
#endif

	png_int_t __CDECL (*p_sigsetjmp)(jmp_buf, png_int_t);
	PNG_NORETURN void __CDECL (*p_longjmp)(jmp_buf, png_int_t);
	
	/* room for later extensions */
	void *unused[31];
};

long __CDECL pnglib_slb_control(long fn, void *arg);

#endif
