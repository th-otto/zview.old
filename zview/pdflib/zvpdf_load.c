#include "../plugins/common/zview.h"
#include <mint/slb.h>
#include <mint/mintbind.h>
#include <errno.h>
#include "../plugins/common/symbols.h"
#include <slb/freetype.h>
#include "../general.h"
#include "../plugins/common/slbload.h"
#include "../plugins/common/zvplugin.h"
#include "../plugins/common/plugin_version.h"
#include "zvpdf.h"
#include "../winimg.h"
#include "../custom_font.h"
#include "pdflib.h"

static struct _zvpdf_funcs zvpdf_funcs;
static SLB zvpdf;

#undef SLB_NWORDS
#undef SLB_NARGS
#if defined(__MSHORT__) || defined(__PUREC__)
#define SLB_NARGS(_nargs) 0, _nargs
#else
#define SLB_NARGS(_nargs) _nargs
#endif

static char zview_slb_dir[MAX_PATH];


boolean __CDECL pdf_init(const char *path)
{
	SLB *slb = &zvpdf;
	return slb->exec(slb->handle, 1, SLB_NARGS(1), path);
}


void __CDECL pdf_exit(void)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 2, SLB_NARGS(0));
}


boolean __CDECL lib_pdf_load(const char *name, IMAGE *img, boolean antialias)
{
	SLB *slb = &zvpdf;
	return slb->exec(slb->handle, 3, SLB_NARGS(3), name, img, antialias);
}


void __CDECL pdf_get_page_size(IMAGE *img, int page, uint16 *width, uint16 *height)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 4, SLB_NARGS(4), img, (long)page, width, height);
}


void __CDECL pdf_decode_page(IMAGE *img, int page, double scale)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 5, SLB_NARGS(4), img, page, scale);
}


uint32 *__CDECL pdf_get_page_addr(IMAGE *img)
{
	SLB *slb = &zvpdf;
	return (uint32 *)slb->exec(slb->handle, 6, SLB_NARGS(1), img);
}


void __CDECL pdf_quit(IMAGE *img)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 7, SLB_NARGS(1), img);
}


const char *__CDECL get_pdf_title(void)
{
	SLB *slb = &zvpdf;
	return (const char *)slb->exec(slb->handle, 8, SLB_NARGS(0));
}


void __CDECL pdf_build_bookmark(WINDATA *windata, WINDOW *win)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 9, SLB_NARGS(2), windata, win);
}


void __CDECL pdf_get_info(IMAGE *img, txt_data *txtdata)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 10, SLB_NARGS(2), img, txtdata);
}


void __CDECL delete_bookmark_child(Bookmark *book)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 11, SLB_NARGS(1), book);
}


long zvpdf_open(void)
{
	SLB *slb = &zvpdf;
	long ret;
	unsigned long flags;
	long cpu = 0;
	static char const name[] = "zvpdf.slb";
	char *end;

	if (slb->handle)
		return 0;

	zvpdf_funcs.struct_size = sizeof(zvpdf_funcs);
	zvpdf_funcs.int_size = sizeof(int);
	if (zvpdf_funcs.int_size != sizeof(long))
		return -EINVAL;
	zvpdf_funcs.plugin_version = PLUGIN_VERSION;
	zvpdf_funcs.p_slb_open = plugin_slb_open;
	zvpdf_funcs.p_slb_close = plugin_slb_close;
	zvpdf_funcs.p_slb_get = plugin_slb_get;
	zvpdf_funcs.p_get_text_width = get_text_width;
	
	strcpy(zview_slb_dir, zview_path);
	strcat(zview_slb_dir, "\\slb");
	end = zview_slb_dir + strlen(zview_slb_dir);
#if defined(__mcoldfire__)
	strcat(zview_slb_dir, "\\v4e");
#elif defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
	strcat(zview_slb_dir, "\\020");
#else
	strcat(zview_slb_dir, "\\000");
#endif
	ret = slb_load(name, zview_slb_dir, PLUGIN_VERSION, &slb->handle, &slb->exec);
	if (ret < 0)
	{
		*end = '\0';
		ret = slb_load(name, zview_slb_dir, PLUGIN_VERSION, &slb->handle, &slb->exec);
	}

	if (ret < 0)
	{
		errshow(name, ret);
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
	
	ret = plugin_set_imports(slb, &zvpdf_funcs);
	if (ret < 0)
	{
		plugin_close(slb);
		return ret;
	}
	
	return ret;
}


void zvpdf_close(void)
{
	SLB *slb = &zvpdf;
	plugin_close(slb);
}
