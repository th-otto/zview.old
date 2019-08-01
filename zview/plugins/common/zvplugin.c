#include "zview.h"
#include <mint/slb.h>
#include <mint/mintbind.h>
#include <errno.h>
#include "symbols.h"
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

static zv_int_t _CDECL get_errno(void)
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


boolean __CDECL plugin_reader_read(SLB *slb, IMGINFO info, uint8 *buffer)
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


boolean __CDECL plugin_encoder_write(SLB *slb, IMGINFO info, uint8 *buffer)
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

#define S(x) zview_plugin_funcs.p_##x = x

	S(memset);
	S(memcpy);
	S(memchr);
	S(strlen);
	S(strcpy);
	S(strcat);
	S(strcmp);
	S(malloc);
	S(free);
	S(get_errno);
	S(strerror);
	S(open);
	S(close);
	S(read);
	S(write);
	S(lseek);
	S(fopen);
	S(fclose);
	S(fseek);
	S(fseeko);
	S(ftell);
	S(ftello);
	S(sprintf);
	S(vsnprintf);
	S(fread);
	S(fwrite);
	S(ferror);
	S(rand);
	S(srand);
#undef S
	
	ret = slb_load(name, path, PLUGIN_VERSION, &slb->handle, &slb->exec);
	if (ret < 0)
		return ret;

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
