/*
 * lzmastruct.h - internal header file.
 * List of functions that are exported from
 * the application to lzma
 *
 * Copyright (C) 2019 Thorsten Otto
 */

#ifndef __LZMASTRUCT_H__
#define __LZMASTRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>

struct _lzma_funcs {
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
	 * version of lzma.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long lzma_vernum;
	
	void *__CDECL (*p_memset)(void *, lzma_int_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	lzma_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	
	size_t __CDECL (*p_strlen)(const char *);
	char *__CDECL (*p_strcpy)(char *, const char *);
	lzma_int_t __CDECL (*p_strcmp)(const char *, const char *);

	void *__CDECL (*p_malloc)(size_t);
	void __CDECL (*p_free)(void *);

	/* room for later extensions */
	void *unused[12];
};

long __CDECL lzma_slb_control(long fn, void *arg);
struct _lzma_funcs *lzma_get_slb_funcs(void);

#endif
