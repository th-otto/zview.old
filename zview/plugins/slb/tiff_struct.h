/*
 * tiffstruct.h - internal header file.
 * List of functions that are exported from
 * the application to tiff
 *
 * Copyright (C) 2019 Thorsten Otto
 */

#ifndef __TIFFSTRUCT_H__
#define __TIFFSTRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <tiffio.h>

struct _tiff_funcs {
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
	 * version of tiff.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long tiff_vernum;
	
	SLB *zlib;
	SLB *jpeg;
	SLB *lzma;
	SLB *jbig;
	SLB *webp;
	SLB *zstd;
	short zlib_opened_here;
	short jpeg_opened_here;
	short lzma_opened_here;
	short jbig_opened_here;
	short webp_opened_here;
	short zstd_opened_here;
	
	long __CDECL (*p_slb_tiff_zlib_open)(const char *slbpath);
	long __CDECL (*p_slb_tiff_jpeg_open)(const char *slbpath);
	long __CDECL (*p_slb_tiff_lzma_open)(const char *slbpath);
	long __CDECL (*p_slb_tiff_jbig_open)(const char *slbpath);
	long __CDECL (*p_slb_tiff_webp_open)(const char *slbpath);
	long __CDECL (*p_slb_tiff_zstd_open)(const char *slbpath);
	
	tiff_int_t __CDECL (*p_get_errno)(void);
	void __CDECL (*p_set_errno)(tiff_int_t e);
	FILE *stderr_location;
	char *__CDECL (*p_strerror)(tiff_int_t);

	void *__CDECL (*p_memset)(void *, tiff_int_t, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	tiff_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	void *__CDECL (*p_memchr)(const void *s, int c, size_t n);
	
	size_t __CDECL (*p_strlen)(const char *);
	char *__CDECL (*p_strcpy)(char *, const char *);
	char *__CDECL (*p_strcat)(char *, const char *);
	tiff_int_t __CDECL (*p_strcmp)(const char *, const char *);
	tiff_int_t __CDECL (*p_strncmp)(const char *, const char *, size_t);

	void *__CDECL (*p_malloc)(size_t);
	void *__CDECL (*p_calloc)(size_t, size_t);
	void *__CDECL (*p_realloc)(void *, size_t);
	void __CDECL (*p_free)(void *);

	tiff_int_t __CDECL (*p_open)(const char *, tiff_int_t, ...);
	tiff_int_t __CDECL (*p_close)(tiff_int_t);
	ssize_t __CDECL (*p_read)(tiff_int_t, void *, size_t);
	ssize_t __CDECL (*p_write)(tiff_int_t, const void *, size_t);
	off_t __CDECL (*p_lseek)(tiff_int_t, off_t, tiff_int_t);

	size_t __CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	tiff_int_t __CDECL (*p_sprintf)(char *, const char *, ...);
	tiff_int_t __CDECL (*p_snprintf)(char *, size_t, const char *, ...);
	tiff_int_t __CDECL (*p_vsnprintf)(char *, size_t, const char *, va_list);
	tiff_int_t __CDECL (*p_fprintf)(FILE *, const char *, ...);
	tiff_int_t __CDECL (*p_vfprintf)(FILE *, const char *, va_list);
	tiff_int_t __CDECL (*p_fputc)(tiff_int_t c, FILE *stream);
	tiff_int_t __CDECL (*p_fputs)(const char *s, FILE *stream);
	
	tiff_int_t __CDECL (*p_rand)(void);
	void __CDECL (*p_srand)(tiff_uint_t seed);
	void __CDECL (*p_qsort)(void *base, size_t nmemb, size_t size, tiff_int_t (*compar)(const void *, const void *));
	void *__CDECL (*p_bsearch)(const void *key, const void *base, size_t nmemb, size_t size, tiff_int_t (*compar)(const void *, const void *));
	char *__CDECL (*p_getenv)(const char *name);
                     
	tiff_int_t __CDECL (*p_fstat)(tiff_int_t fd, struct stat *s);

	tiff_int_t __CDECL (*p_sigsetjmp)(jmp_buf, tiff_int_t);
	__attribute__((__noreturn__)) void __CDECL (*p_longjmp)(jmp_buf, tiff_int_t);

#if 0
	/* functions forwarded to jpeglib */
	char *___CDECL (*p_strncpy)(char *, const char *, size_t);
	size_t ___CDECL (*p_fread)(void *, size_t, size_t, FILE *);
	tiff_int_t ___CDECL (*p_fflush)(FILE *);
	tiff_int_t ___CDECL (*p_ferror)(FILE *);
	void ___CDECL (*p_exit)(tiff_int_t);
#endif

	TIFFErrorHandler _TIFFwarningHandler;
	TIFFErrorHandlerExt _TIFFwarningHandlerExt;
	TIFFErrorHandler _TIFFerrorHandler;
	TIFFErrorHandlerExt _TIFFerrorHandlerExt;
	TIFFExtendProc _TIFFextender;
	
	/* room for later extensions */
	void *unused[67];
};

long __CDECL tiff_slb_control(long fn, void *arg);
struct _tiff_funcs *tiff_get_slb_funcs(void);

#endif
