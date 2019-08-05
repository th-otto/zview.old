/*
 * zlib_imp_open.c - loader for library
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#include "slb/zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <mint/cookie.h>
#include <mint/mintbind.h>
#include "zlib_struct.h"
#include "../common/slbload.h"
#include "symbols.h"

static struct _zlibslb_funcs zlibslb_funcs;


typedef struct
{
	long key;
	long value;
} COOKIE;


static COOKIE *get_cookie_jar(void)
{
	return (COOKIE *) Setexc(360, (void (*)()) -1L);
}


static int get_cookie(long key, long *value)
{
	COOKIE *cookiejar = get_cookie_jar();

	*value = 0;
	if (cookiejar)
	{
		while (cookiejar->key)
		{
			if (cookiejar->key == key)
			{
				*value = cookiejar->value;
				return 1;
			}
			cookiejar++;
		}
	}
	return 0;
}


static z_int_t __CDECL get_errno(void)
{
	return errno;
}

#define S(x) zlibslb_funcs.p_##x = x


long slb_zlib_open(const char *slbpath)
{
	long ret;
	unsigned long flags;
	long cpu;
	
	SLB *zlib = slb_zlib_get();
	
	if (!zlib)
		return -EBADF;
	if (zlib->handle)
		return 0;

	zlibslb_funcs.struct_size = sizeof(zlibslb_funcs);
	zlibslb_funcs.int_size = sizeof(z_int_t);
	if (zlibslb_funcs.int_size != sizeof(long))
		return -EINVAL;
	zlibslb_funcs.zlib_vernum = ZLIB_VERNUM;
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
	
	ret = slb_load(ZLIB_SHAREDLIB_NAME, slbpath, ZLIB_VERNUM, &zlib->handle, &zlib->exec);
	if (ret < 0)
		return ret;

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = zlib_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_zlib_close();
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & (1L << 17)) || cpu > 0)
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & (1L << 17))
#endif
	{
		slb_zlib_close();
		return -EINVAL;
	}
	
	ret = zlib_slb_control(1, &zlibslb_funcs);
	if (ret < 0)
	{
		slb_zlib_close();
		return ret;
	}
	
	return ret;
}


LIBFUNC(zlib_slb_control, slb_zlib_get)
