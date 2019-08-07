/*
 * exifstruct.h - internal header file.
 * List of functions that are exported from
 * the application to exif
 *
 * Copyright (C) 2019 Thorsten Otto
 */

#ifndef __EXIFSTRUCT_H__
#define __EXIFSTRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <libexif/exif-data.h>

struct _exif_funcs {
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
	 * version of exif the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long exif_vernum;
	
	void *__CDECL (*p_memset)(void *, exif_int_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	exif_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	void *__CDECL (*p_memchr)(const void *s, int c, size_t n);
	
	size_t __CDECL (*p_strlen)(const char *);
	char *__CDECL (*p_strcpy)(char *, const char *);
	char *__CDECL (*p_strncpy)(char *, const char *, size_t);
	char *__CDECL (*p_strcat)(char *, const char *);
	char *__CDECL (*p_strncat)(char *, const char *, size_t);
	exif_int_t __CDECL (*p_strcmp)(const char *, const char *);
	exif_int_t __CDECL (*p_strncmp)(const char *, const char *, size_t);

	void *__CDECL (*p_malloc)(size_t);
	void *__CDECL (*p_calloc)(size_t, size_t);
	void *__CDECL (*p_realloc)(void *, size_t);
	void __CDECL (*p_free)(void *);

	FILE *__CDECL (*p_fopen)(const char *pathname, const char *mode);
	exif_int_t __CDECL (*p_fclose)(FILE *stream);
	size_t __CDECL (*p_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	exif_int_t __CDECL (*p_sprintf)(char *, const char *, ...);
	exif_int_t __CDECL (*p_snprintf)(char *, size_t, const char *, ...);
	exif_int_t __CDECL (*p_printf)(const char *, ...);
	
	void __CDECL (*p_qsort)(void *base, size_t nmemb, size_t size, exif_int_t (*compar)(const void *, const void *));
	void *__CDECL (*p_bsearch)(const void *key, const void *base, size_t nmemb, size_t size, exif_int_t (*compar)(const void *, const void *));
                     
	time_t __CDECL (*p_time)(time_t *);
	struct tm *__CDECL (*p_localtime)(const time_t *timep);
	
	/* room for later extensions */
	void *unused[36];
};

long __CDECL exif_slb_control(long fn, void *arg);
struct _exif_funcs *exif_get_slb_funcs(void);

#endif
