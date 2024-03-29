#include "plugin.h"
#include "zvplugin.h"
#include <gem.h>
#undef EXTERN
#undef LOCAL
#undef GLOBAL
#include "jpgdh.h"
#include "jpgdsp.h"
#include "zvjpg.h"

int16_t dsp_ram = 0;

JPGDDRV_PTR jpgdrv = NULL;

#ifdef __GNUC__
static JPGD_ENUM JPGDOpenDriver(JPGD_PTR jpgd, JPGDDRV_PTR drv)
{
	register int32_t retv __asm__("d0");
	register JPGD_ENUM	(*func)(JPGD_PTR) __asm__("a1") = drv->JPGDOpenDriver;
	__asm__ volatile(
	"	movl	%[jpgd],%%a0\n"
	"	jsr		(%[func])\n"
	:	"=r"	(retv)		/* out */
	:	[jpgd]"a"		(jpgd)		/* in */
	,	[func]"a"		(func)		/* in */
	:	__CLOBBER_RETURN("d0") "a0", "d1", "d2", "cc", "memory"
	);
	return retv;
}

static JPGD_ENUM JPGDCloseDriver(JPGD_PTR jpgd, JPGDDRV_PTR drv)
{
	register int32_t retv __asm__("d0");
	register JPGD_ENUM	(*func)(JPGD_PTR) __asm__("a1") = drv->JPGDCloseDriver;
	__asm__ volatile(
	"	movl	%[jpgd],%%a0\n"
	"	jsr		(%[func])\n"
	:	"=r"	(retv)		/* out */
	:	[jpgd]"a"		(jpgd)		/* in */
	,	[func]"a"		(func)	/* in */
	:	__CLOBBER_RETURN("d0") "a0", "d1", "d2", "cc", "memory"
	);
	return retv;
}

static JPGD_ENUM JPGDGetImageInfo(JPGD_PTR jpgd, JPGDDRV_PTR drv)
{
	register int32_t retv __asm__("d0");
	register JPGD_ENUM	(*func)(JPGD_PTR) __asm__("a1") = drv->JPGDGetImageInfo;
	__asm__ volatile(
	"	movl	%[jpgd],%%a0\n"
	"	jsr		(%[func])\n"
	:	"=r"	(retv)		/* out */
	:	[jpgd]"a"		(jpgd)		/* in */
	,	[func]"a"		(func)		/* in */
	:	__CLOBBER_RETURN("d0") "a0", "d1", "d2", "cc", "memory"
	);
	return retv;
}

static JPGD_ENUM JPGDGetImageSize(JPGD_PTR jpgd, JPGDDRV_PTR drv)
{
	register int32_t retv __asm__("d0");
	register JPGD_ENUM	(*func)(JPGD_PTR) __asm__("a1") = drv->JPGDGetImageSize;
	__asm__ volatile(
	"	movl	%[jpgd],%%a0\n"
	"	jsr		(%[func])\n"
	:	"=r"	(retv)		/* out */
	:	[jpgd]"a"		(jpgd)		/* in */
	,	[func]"a"		(func)		/* in */
	:	__CLOBBER_RETURN("d0") "a0", "d1", "d2", "cc", "memory"
	);
	return retv;
}

static JPGD_ENUM JPGDDecodeImage(JPGD_PTR jpgd, JPGDDRV_PTR drv)
{
	register int32_t retv __asm__("d0");
	register JPGD_ENUM	(*func)(JPGD_PTR) __asm__("a1") = drv->JPGDDecodeImage;
	__asm__ volatile(
	"	movl	%[jpgd],%%a0\n"
	"	jsr		(%[func])\n"
	:	"=r"	(retv)		/* out */
	:	[jpgd]"a"		(jpgd)		/* in */
	,	[func]"a"		(func)	/* in */
	:	__CLOBBER_RETURN("d0") "a0", "d1", "d2", "cc", "memory"
	);
	return retv;
}
#endif /* __GNUC__ */

#ifdef __PUREC__
#define JPGDOpenDriver(jpgd, drv) drv->JPGDOpenDriver(jpgd)
#define JPGDCloseDriver(jpgd, drv) drv->JPGDCloseDriver(jpgd)
/* #define JPGDGetStructSize(drv) drv->JPGDGetStructSize() */
#define JPGDGetImageInfo(jpgd, drv) drv->JPGDGetImageInfo(jpgd)
#define JPGDGetImageSize(jpgd, drv) drv->JPGDGetImageSize(jpgd)
#define JPGDDecodeImage(jpgd, drv) drv->JPGDDecodeImage(jpgd)
#endif

/*==================================================================================*
 * boolean CDECL reader_init:														*
 *		Open the file "name", fit the "info" struct. ( see zview.h) and make others	*
 *		things needed by the decoder.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name		->	The file to open.											*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
int16_t reader_dsp_init( const char *name, IMGINFO info)
{
	static char	const pad[] = { -1, -1, -1, -1, -1, -1, -1, -1, 0, 0 };
	void		*src, *dst;
	int16_t		jpeg_file;
	int32_t		jpgdsize, jpeg_file_size;
	JPGD_PTR 	jpgd;

	if ( ( jpeg_file = ( int16_t)Fopen( name, 0)) < 0)
		return GLOBAL_ERROR;

	jpeg_file_size = Fseek( 0L, jpeg_file, 2);

	Fseek( 0L, jpeg_file, 0);

	if (( src = ( void*)Mxalloc( jpeg_file_size + sizeof( pad), dsp_ram)) == NULL)
	{
		Fclose( jpeg_file);
		return GLOBAL_ERROR;
	}

	if ( Fread( jpeg_file, jpeg_file_size, src) != jpeg_file_size)
	{
		Mfree( src);
		Fclose( jpeg_file);
		return GLOBAL_ERROR;
	}

	Fclose( jpeg_file);

	memcpy( ( uint8_t *)src + jpeg_file_size, pad, sizeof( pad));

	jpgdsize = jpgdrv->JPGDGetStructSize();

	if( jpgdsize <= 0)
	{
		Mfree( src);
		return DSP_ERROR;
	}	   
	   
	jpgd = ( JPGD_PTR)Mxalloc( jpgdsize, dsp_ram);

	if( jpgd == NULL)
	{
		Mfree( src);
		return GLOBAL_ERROR;
	}

	memset( ( void *)jpgd, 0, jpgdsize);

	if( JPGDOpenDriver( jpgd, jpgdrv) != 0)
	{
		Mfree( jpgd);
		Mfree( src);
		return DSP_ERROR;
	}

	jpgd->InPointer = src;
	jpgd->InSize    = jpeg_file_size;

	if( JPGDGetImageInfo( jpgd, jpgdrv) != 0)
	{
		JPGDCloseDriver( jpgd, jpgdrv);
		Mfree( jpgd);
		Mfree( src);
		return DSP_ERROR;
	}

	if (jpgd->InComponents == 1)
	{
		jpgd->OutComponents = 1;
		jpgd->OutPixelSize  = 1;
	} else
	{
		jpgd->OutComponents = 3;
		jpgd->OutPixelSize  = 3;
	}

	if( JPGDGetImageSize( jpgd, jpgdrv) != 0)
	{
		JPGDCloseDriver( jpgd, jpgdrv);
		Mfree( jpgd);
		Mfree( src);
		return DSP_ERROR;
	}

	if(( dst = ( void*)Mxalloc( jpgd->OutSize, dsp_ram)) == NULL)
	{
		JPGDCloseDriver( jpgd, jpgdrv);
		Mfree( jpgd);
		Mfree( src);
		return GLOBAL_ERROR;
	}

	jpgd->OutPointer  = dst;
	jpgd->OutFlag     = 0;			 
	jpgd->UserRoutine = NULL;

	if( JPGDDecodeImage( jpgd, jpgdrv) != 0)
	{
		JPGDCloseDriver( jpgd, jpgdrv);
		Mfree( jpgd);
		Mfree( src);
		Mfree( dst);
		return DSP_ERROR;
	}

	info->components 			= jpgd->OutComponents;
	info->width   				= jpgd->MFDBStruct.fd_w;
	info->height  				= jpgd->MFDBStruct.fd_h;
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->planes   				= info->components << 3;
	info->orientation 			= UP_TO_DOWN;
	info->colors  				= 1uL << info->planes;
	info->indexed_color 		= FALSE;
	info->page	 				= 1;
	info->delay		 			= 0;
	info->num_comments			= 0;

	info->_priv_ptr				= dst;	
	info->_priv_var				= jpgd->MFDBStruct.fd_wdwidth << 1;
	info->_priv_var_more		= info->_priv_var;

	strcpy( info->info, "JPEG");
	if (info->components == 1)
		strcat( info->info, " Greyscale");
	strcpy( info->compression, "JPG");	

	JPGDCloseDriver( jpgd, jpgdrv);

	Mfree( jpgd);
	Mfree( src);

	return ALL_OK;
}
