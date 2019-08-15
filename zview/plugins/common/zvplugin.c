#include "zview.h"
#include <mint/slb.h>
#include <mint/mintbind.h>
#include <errno.h>
#include "symbols.h"
#include <slb/png.h>
#include <slb/zlib.h>
#include <slb/jpeg.h>
#include <slb/tiff.h>
#include <slb/lzma.h>
#include <slb/exif.h>
#include <slb/bzip2.h>
#include <slb/freetype.h>
#include "zvplugin.h"
#include "plugin.h"
#include "plugin_version.h"
#include "slbload.h"

static struct _zview_plugin_funcs zview_plugin_funcs;

#undef SLB_NWORDS
#undef SLB_NARGS
#if defined(__MSHORT__) || defined(__PUREC__)
#define SLB_NARGS(_nargs) 0, _nargs
#else
#define SLB_NARGS(_nargs) _nargs
#endif

#undef errno

static zv_int_t __CDECL get_errno(void)
{
	return errno;
}


#if 0
#pragma GCC optimize "-fno-defer-pop"

static long __inline __attribute__((__always_inline__)) get_sp(void)
{
	long r;
	asm volatile("move.l %%sp,%0" : "=r"(r) : : "cc");
	return r;
}
#endif


long __CDECL plugin_slb_control(SLB *slb, long fn, void *arg)
{
	return slb->exec(slb->handle, 0, SLB_NARGS(2), fn, arg);
}

	
boolean __CDECL plugin_reader_init(SLB *slb, const char *name, IMGINFO info)
{
	return slb->exec(slb->handle, 1, SLB_NARGS(2), name, info) > 0;
}


boolean __CDECL plugin_reader_read(SLB *slb, IMGINFO info, uint8_t *buffer)
{
	return slb->exec(slb->handle, 2, SLB_NARGS(2), info, buffer) > 0;
}


void __CDECL plugin_reader_get_txt(SLB *slb, IMGINFO info, txt_data *txtdata)
{
	slb->exec(slb->handle, 3, SLB_NARGS(2), info, txtdata);
}


void __CDECL plugin_reader_quit(SLB *slb, IMGINFO info)
{
	slb->exec(slb->handle, 4, SLB_NARGS(1), info);
}


boolean __CDECL plugin_encoder_init(SLB *slb, const char *name, IMGINFO info)
{
	return slb->exec(slb->handle, 5, SLB_NARGS(2), name, info) > 0;
}


boolean __CDECL plugin_encoder_write(SLB *slb, IMGINFO info, uint8_t *buffer)
{
	return slb->exec(slb->handle, 6, SLB_NARGS(2), info, buffer) > 0;
}


void __CDECL plugin_encoder_quit(SLB *slb, IMGINFO info)
{
	slb->exec(slb->handle, 7, SLB_NARGS(1), info);
}


long __CDECL plugin_get_option(SLB *slb, zv_int_t which)
{
	return slb->exec(slb->handle, 8, SLB_NARGS(1), which);
}


long __CDECL plugin_set_option(SLB *slb, zv_int_t which, zv_int_t value)
{
	return slb->exec(slb->handle, 9, SLB_NARGS(2), which, value);
}


static long __CDECL slb_not_loaded(SLB_HANDLE slb, long fn, short nwords, ...)
{
	(void)slb;
	(void)fn;
	(void)nwords;
	(void) Cconws("a shared lib was not loaded\r\n");
	Pterm(1);
	return -32;
}


static long __CDECL slb_unloaded(SLB_HANDLE slb, long fn, short nwords, ...)
{
	(void)slb;
	(void)fn;
	(void)nwords;
	(void) Cconws("a shared lib was already unloaded\r\n");
	Pterm(1);
	return -32;
}


static SLB zlib_slb = { 0, slb_not_loaded };

SLB *slb_zlib_get(void)
{
	return &zlib_slb;
}


void slb_zlib_close(void)
{
	SLB *zlib = slb_zlib_get();

	if (!zlib->handle)
		return;
	slb_unload(zlib->handle);
	zlib->handle = 0;
	zlib->exec = slb_unloaded;
}


static SLB pnglib_slb = { 0, slb_not_loaded };

SLB *slb_pnglib_get(void)
{
	return &pnglib_slb;
}


void slb_pnglib_close(void)
{
	SLB *pnglib = slb_pnglib_get();

	if (!pnglib->handle)
		return;
	slb_unload(pnglib->handle);
	pnglib->handle = 0;
	pnglib->exec = slb_unloaded;
}


static SLB jpeglib_slb = { 0, slb_not_loaded };

SLB *slb_jpeglib_get(void)
{
	return &jpeglib_slb;
}


void slb_jpeglib_close(void)
{
	SLB *jpeglib = slb_jpeglib_get();

	if (!jpeglib->handle)
		return;
	slb_unload(jpeglib->handle);
	jpeglib->handle = 0;
	jpeglib->exec = slb_unloaded;
}


static SLB exif_slb = { 0, slb_not_loaded };

SLB *slb_exif_get(void)
{
	return &exif_slb;
}


void slb_exif_close(void)
{
	SLB *exif = slb_exif_get();

	if (!exif->handle)
		return;
	slb_unload(exif->handle);
	exif->handle = 0;
	exif->exec = slb_unloaded;
}


static SLB tiff_slb = { 0, slb_not_loaded };

SLB *slb_tiff_get(void)
{
	return &tiff_slb;
}


void slb_tiff_close(void)
{
	SLB *tiff = slb_tiff_get();

	if (!tiff->handle)
		return;
	slb_unload(tiff->handle);
	tiff->handle = 0;
	tiff->exec = slb_unloaded;
}


static SLB lzma_slb = { 0, slb_not_loaded };

SLB *slb_lzma_get(void)
{
	return &lzma_slb;
}


void slb_lzma_close(void)
{
	SLB *lzma = slb_lzma_get();

	if (!lzma->handle)
		return;
	slb_unload(lzma->handle);
	lzma->handle = 0;
	lzma->exec = slb_unloaded;
}


static SLB bzip2_slb = { 0, slb_not_loaded };

SLB *slb_bzip2_get(void)
{
	return &bzip2_slb;
}


void slb_bzip2_close(void)
{
	SLB *bzip2 = slb_bzip2_get();

	if (!bzip2->handle)
		return;
	slb_unload(bzip2->handle);
	bzip2->handle = 0;
	bzip2->exec = slb_unloaded;
}


static SLB freetype_slb = { 0, slb_not_loaded };

SLB *slb_freetype_get(void)
{
	return &freetype_slb;
}


void slb_freetype_close(void)
{
	SLB *freetype = slb_freetype_get();

	if (!freetype->handle)
		return;
	slb_unload(freetype->handle);
	freetype->handle = 0;
	freetype->exec = slb_unloaded;
}


long __CDECL plugin_slb_open(zv_int_t lib)
{
	switch (lib)
	{
	case LIB_PNG:
		return slb_pnglib_open(NULL);
	case LIB_Z:
		return slb_zlib_open(NULL);
	case LIB_JPEG:
		return slb_jpeglib_open(NULL);
	case LIB_TIFF:
		return slb_tiff_open(NULL);
	case LIB_LZMA:
		return slb_lzma_open(NULL);
	case LIB_EXIF:
		return slb_exif_open(NULL);
	case LIB_BZIP2:
		return slb_bzip2_open(NULL);
	case LIB_FREETYPE:
		return slb_freetype_open(NULL);
	}
	return -ENOENT;
}


void __CDECL plugin_slb_close(zv_int_t lib)
{
	switch (lib)
	{
	case LIB_PNG:
		slb_pnglib_close();
		break;
	case LIB_Z:
		slb_zlib_close();
		break;
	case LIB_JPEG:
		slb_jpeglib_close();
		break;
	case LIB_TIFF:
		slb_tiff_close();
		break;
	case LIB_LZMA:
		slb_lzma_close();
		break;
	case LIB_EXIF:
		slb_exif_close();
		break;
	case LIB_BZIP2:
		slb_bzip2_close();
		break;
	case LIB_FREETYPE:
		slb_freetype_close();
		break;
	}
}


SLB *__CDECL plugin_slb_get(zv_int_t lib)
{
	switch (lib)
	{
	case LIB_PNG:
		return slb_pnglib_get();
	case LIB_Z:
		return slb_zlib_get();
	case LIB_JPEG:
		return slb_jpeglib_get();
	case LIB_TIFF:
		return slb_tiff_get();
	case LIB_LZMA:
		return slb_lzma_get();
	case LIB_EXIF:
		return slb_exif_get();
	case LIB_BZIP2:
		return slb_bzip2_get();
	case LIB_FREETYPE:
		return slb_freetype_get();
	}
	return NULL;
}


long plugin_open(const char *name, const char *path, SLB *slb)
{
	long ret;
	unsigned long flags;
	long cpu = 0;

	if (!slb)
		return -EBADF;
	if (slb->handle)
		return 0;

	zview_plugin_funcs.struct_size = sizeof(zview_plugin_funcs);
	zview_plugin_funcs.int_size = sizeof(zv_int_t);
	if (zview_plugin_funcs.int_size != sizeof(long))
		return -EINVAL;
	zview_plugin_funcs.plugin_version = PLUGIN_VERSION;
	zview_plugin_funcs.p_slb_open = plugin_slb_open;
	zview_plugin_funcs.p_slb_close = plugin_slb_close;
	zview_plugin_funcs.p_slb_get = plugin_slb_get;

#define S(x) zview_plugin_funcs.p_##x = x

	S(memset);
	S(memcpy);
	S(memchr);
	S(memcmp);

	S(strlen);
	S(strcpy);
	S(strncpy);
	S(strcat);
	S(strncat);
	S(strcmp);
	S(strncmp);

	S(malloc);
	S(calloc);
	S(realloc);
	S(free);

	S(get_errno);
	S(strerror);
	S(abort);
	zview_plugin_funcs.stderr_location = stderr;
	
	S(remove);

	S(open);
	S(close);
	S(read);
	S(write);
	S(lseek);

	S(fopen);
	S(fdopen);
	S(fclose);
	S(fseek);
	S(fseeko);
	S(ftell);
	S(ftello);
	S(printf);
	S(sprintf);
	S(vsnprintf);
	S(vfprintf);
	S(fread);
	S(fwrite);
	S(ferror);
	S(fflush);

	S(rand);
	S(srand);

	S(qsort);
	S(bsearch);

	S(time);
	S(localtime);
	S(gmtime);

	S(fstat);

	S(sigsetjmp);
	S(longjmp);

	S(atof);
#undef S

	ret = slb_load(name, path, PLUGIN_VERSION, &slb->handle, &slb->exec);
	if (ret < 0)
	{
		slb->handle = 0;
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = plugin_compile_flags(slb);
	Getcookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			plugin_close(slb);
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & (1L << 17)))
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & (1L << 17))
#endif
	{
		plugin_close(slb);
		return -EINVAL;
	}
	
	ret = plugin_set_imports(slb, &zview_plugin_funcs);
	if (ret < 0)
	{
		plugin_close(slb);
		return ret;
	}
	
	return ret;
}


void plugin_close(SLB *slb)
{
	if (!slb || !slb->handle)
		return;
	slb_unload(slb->handle);
	slb->handle = 0;
	slb->exec = 0;
}
