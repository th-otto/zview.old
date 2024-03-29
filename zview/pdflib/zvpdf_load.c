#include "../plugins/common/zview.h"
#include <mint/slb.h>
#include <mint/mintbind.h>
#include <errno.h>
#include "../plugins/common/symbols.h"
#include <slb/freetype.h>
#include "../general.h"
#include "../plugins/common/slbload.h"
#include "../plugins/common/zvplugin.h"
#include "../plugins/common/plugver.h"
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


#if 0
const char *__CDECL get_pdf_title(void)
{
	SLB *slb = &zvpdf;
	return (const char *)slb->exec(slb->handle, 8, SLB_NARGS(0));
}
#endif


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


void __CDECL delete_bookmarks(WINDATA *windata)
{
	SLB *slb = &zvpdf;
	slb->exec(slb->handle, 11, SLB_NARGS(1), windata);
}


#define DBG_ALLOC 0

#if DBG_ALLOC
#include <mint/arch/nf_ops.h>

struct alloc {
	struct alloc *next;
};
static struct alloc *alloc_list;

static int check_alloc(struct alloc *pptr, const char *where)
{
	struct alloc **p;
	
	p = &alloc_list;
	while (*p)
	{
		if (*p == pptr)
			return TRUE;
		p = &(*p)->next;
	}
#if DBG_ALLOC >= 2
	nf_debugprintf("%s: ptr not on list: %08lx\n", where, (unsigned long)(pptr + 1));
#endif
	return FALSE;
}


static int remove_alloc(struct alloc *pptr, const char *where)
{
	struct alloc **p;
	
	p = &alloc_list;
	while (*p)
	{
		if (*p == pptr)
		{
			*p = pptr->next;
			return 1;
		}
		p = &(*p)->next;
	}
#if DBG_ALLOC >= 2
	nf_debugprintf("%s: ptr not on list: %08lx\n", where, (unsigned long)(pptr + 1));
#endif
	return FALSE;
}


static void *my_malloc(size_t len)
{
	struct alloc *p;

	p = malloc(len + sizeof(struct alloc));
	if (p)
	{
		p->next = alloc_list;
		alloc_list = p;
		++p;
	}
#if DBG_ALLOC >= 3
	nf_debugprintf("malloc %lu: %08lx\n", len, (unsigned long)(p));
#endif
	return p;
}

static void *my_realloc(void *ptr, size_t len)
{
	struct alloc *p;
	struct alloc *pptr;

	pptr = ptr;
	if (pptr)
	{
		pptr--;
		if (!check_alloc(pptr, "realloc"))
			return NULL;
	}
	p = realloc(pptr, len + sizeof(struct alloc));
	if (p)
	{
		if (p != pptr)
		{
			if (pptr)
				remove_alloc(pptr, "realloc");
			p->next = alloc_list;
			alloc_list = p;
		}
		++p;
	} else if (pptr)
	{
		remove_alloc(pptr, "realloc");
	}
#if DBG_ALLOC >= 3
	nf_debugprintf("realloc %08lx %lu: %08lx\n", (unsigned long)ptr, len, (unsigned long)(p));
#endif
	return p;
}

static void my_free(void *p)
{
	struct alloc *pptr;
	if (p == NULL)
		return;
	pptr = p;
	pptr--;
#if DBG_ALLOC >= 3
	nf_debugprintf("free %08lx\n", (unsigned long)(pptr + 1));
#endif
	if (remove_alloc(pptr, "free"))
		free(pptr);
}
#endif


long zvpdf_open(void)
{
	SLB *slb = &zvpdf;
	long ret;
	unsigned long flags;
	long cpu = 0;
	static char const name[] = "zvpdf.slb";

	if (slb->handle)
		return 0;

	zvpdf_funcs.struct_size = sizeof(zvpdf_funcs);
	zvpdf_funcs.int_size = sizeof(int);
	if (zvpdf_funcs.int_size != sizeof(long))
		return -EINVAL;
	zvpdf_funcs.interface_version = XPDF_INTERFACE_VERSION;
	zvpdf_funcs.p_slb_open = plugin_slb_open;
	zvpdf_funcs.p_slb_close = plugin_slb_close;
	zvpdf_funcs.p_slb_get = plugin_slb_get;
	
	zvpdf_funcs.stderr_location = stderr;

#define S(x) zvpdf_funcs.p_ ## x = x

	S(get_text_width);

	S(malloc);
	S(free);
	S(realloc);

	S(memcmp);
	S(memcpy);
	S(memset);

	S(strlen);
	S(strcmp);
	S(strncmp);
	S(strcpy);
	S(strncpy);
	S(strtok);
	S(strstr);
	S(strchr);
	S(strrchr);
	S(strcasecmp);
	S(strncasecmp);
	S(strcspn);
	S(strtoul);

	S(getcwd);
	S(qsort);
	S(srand);
	S(rand);
	S(getenv);
	S(tmpnam);
	S(mkstemp);
	
	S(mktime);
	S(strftime);
	
	S(isspace);
	S(isalnum);
	S(isalpha);
	S(isxdigit);
	S(isupper);
	S(islower);
	S(toupper);
	S(tolower);

	S(vsprintf);
	S(vfprintf);
	S(fflush);
	S(fputs);
	S(fputc);
	S(fopen);
	S(fclose);
	S(fread);
	S(fwrite);
	S(fseek);
	S(ftell);
	S(unlink);
	S(vsscanf);
	S(atoi);
	S(atof);
	S(fgetc);
	S(ungetc);
	S(fdopen);
	
#undef S

#if DBG_ALLOC
	zvpdf_funcs.p_malloc = my_malloc;
	zvpdf_funcs.p_realloc = my_realloc;
	zvpdf_funcs.p_free = my_free;
#endif

	ret = slb_load(name, zview_slb_dir, PLUGIN_INTERFACE_VERSION, &slb->handle, &slb->exec);
	if (ret < 0)
	{
		char c = *zview_slb_dir_end;
		*zview_slb_dir_end = '\0';
		ret = slb_load(name, zview_slb_dir, PLUGIN_INTERFACE_VERSION, &slb->handle, &slb->exec);
		*zview_slb_dir_end = c;
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
		if (flags & SLB_M68020)
		{
			/* cpu is not 020+, but library was compiled for it */
			plugin_close(slb, FALSE);
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & SLB_COLDFIRE))
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & SLB_COLDFIRE)
#endif
	{
		plugin_close(slb, FALSE);
		return -EINVAL;
	}
	
	ret = plugin_set_imports(slb, &zvpdf_funcs);
	if (ret < 0)
	{
		plugin_close(slb, FALSE);
		return ret;
	}
	
	return ret;
}


void zvpdf_close(void)
{
	SLB *slb = &zvpdf;
#if DBG_ALLOC >= 2
	{
		struct alloc *p;
		
		for (p = alloc_list; p; p = p->next)
		{
			nf_debugprintf("not freed: %08lx\n", (unsigned long)(p + 1));
		}
	}
#endif
	plugin_close(slb, FALSE);
}
