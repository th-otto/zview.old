#include "zview.h"
#include "jpgdh.h"

int16 dsp_ram = 0;

int16 reader_dsp_init( const char *name, IMGINFO info);

JPGDDRV_PTR jpgdrv = NULL;

#define	JPGDOpenDriver( jpgd, jpgdrv)		\
__extension__								\
({	register int32 retv __asm__("d0");		\
	__asm__ volatile						\
	("	movl	%1,a0;						\
		movl	(%2,4),a1;					\
		jsr		a1@;						\
	"	/* end of code */					\
	:	"=r"	(retv)		/* out */		\
	:	"a"		(jpgd)		/* in */		\
	,	"a"		(jpgdrv)	/* in */		\
	:	"a0", "a1"							\
	);										\
	retv;									\
})

#define	JPGDCloseDriver( jpgd, jpgdrv)		\
__extension__								\
({	register int32 retv __asm__("d0");		\
	__asm__ volatile						\
	("	movl	%1,a0;						\
		movl	(%2,8),a1;					\
		jsr		a1@;						\
	"	/* end of code */					\
	:	"=r"	(retv)		/* out */		\
	:	"a"		(jpgd)		/* in */		\
	,	"a"		(jpgdrv)	/* in */		\
	:	"a0", "a1"							\
	);										\
	retv;									\
})

#define	JPGDGetImageInfo( jpgd, jpgdrv)		\
__extension__								\
({	register int32 retv __asm__("d0");		\
	__asm__ volatile						\
	("	movl	%1,a0;						\
		movl	(%2,16),a1;					\
		jsr		a1@;						\
	"	/* end of code */					\
	:	"=r"	(retv)		/* out */		\
	:	"a"		(jpgd)		/* in */		\
	,	"a"		(jpgdrv)	/* in */		\
	:	"a0", "a1"							\
	);										\
	retv;									\
})

#define	JPGDGetImageSize( jpgd, jpgdrv)		\
__extension__								\
({	register int32 retv __asm__("d0");		\
	__asm__ volatile						\
	("	movl	%1,a0;						\
		movl	(%2,20),a1;					\
		jsr		a1@;						\
	"	/* end of code */					\
	:	"=r"	(retv)		/* out */		\
	:	"a"		(jpgd)		/* in */		\
	,	"a"		(jpgdrv)	/* in */		\
	:	"a0", "a1"							\
	);										\
	retv;									\
})

#define	JPGDDecodeImage( jpgd, jpgdrv)		\
__extension__								\
({	register int32 retv __asm__("d0");		\
	__asm__ volatile						\
	("	movl	%1,a0;						\
		movl	(%2,24),a1;					\
		jsr		a1@;						\
	"	/* end of code */					\
	:	"=r"	(retv)		/* out */		\
	:	"a"		(jpgd)		/* in */		\
	,	"a"		(jpgdrv)	/* in */		\
	:	"a0", "a1"							\
	);										\
	retv;									\
})

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
int16 reader_dsp_init( const char *name, IMGINFO info)
{
	char		pad[] = { -1, -1, -1, -1, -1, -1, -1, -1, 0, 0};
	void		*src, *dst;
	int16		jpeg_file;
	int32		jpgdsize, jpeg_file_size;
	JPGD_PTR 	jpgd;

	if ( ( jpeg_file = ( int16)Fopen( name, 0)) < 0)
		return GOLBAL_ERROR;

	jpeg_file_size = Fseek( 0L, jpeg_file, 2);

	Fseek( 0L, jpeg_file, 0);

	if (( src = ( void*)Mxalloc( jpeg_file_size + sizeof( pad), dsp_ram)) == NULL)
	{
		Fclose( jpeg_file);
		return GOLBAL_ERROR;	
	}

	if ( Fread( jpeg_file, jpeg_file_size, src) != jpeg_file_size)
	{
		Mfree( src);
		Fclose( jpeg_file);
		return GOLBAL_ERROR;	
	}

	Fclose( jpeg_file);

	memcpy( ( uint8 *)src + jpeg_file_size, pad, sizeof( pad));

	jpgdsize = jpgdrv->JPGDGetStructSize();

	if( jpgdsize < 1)
	{
		Mfree( src);
		return DSP_ERROR;
	}	   
	   
	jpgd = ( JPGD_PTR)Mxalloc( jpgdsize, dsp_ram);

	if( jpgd == NULL)
	{
		Mfree( src);
		return GOLBAL_ERROR;
	}

	memset( ( void *)jpgd, 0, jpgdsize);

	if( JPGDOpenDriver( jpgd, jpgdrv) != 0)
	{
		Dsp_Unlock();
		Mfree( jpgd);
		Mfree( src);
		return DSP_ERROR;
	}

	jpgd->InPointer = src;
	jpgd->InSize    = jpeg_file_size;

	if( JPGDGetImageInfo( jpgd, jpgdrv) != 0)
	{
		JPGDCloseDriver( jpgd, jpgdrv);
		Dsp_Unlock();
		Mfree( jpgd);
		Mfree( src);
		return DSP_ERROR;
	}

	jpgd->OutComponents = 3;
	jpgd->OutPixelSize  = 3;

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
		return GOLBAL_ERROR;
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

	info->components 			= 3;
	info->width   				= jpgd->MFDBStruct.fd_w;
	info->height  				= jpgd->MFDBStruct.fd_h;
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->planes   				= 24;
	info->orientation 			= UP_TO_DOWN;
	info->colors  				= 1uL << ( uint32)info->planes;
	info->indexed_color 		= FALSE;
	info->page	 				= 1;
	info->delay		 			= 0;
	info->num_comments			= 0;
	info->max_comments_length	= 0;

	info->_priv_ptr				= dst;	
	info->_priv_var				= jpgd->MFDBStruct.fd_wdwidth << 1;
	info->_priv_var_more		= info->_priv_var;

	strcpy( info->info, "JPEG");
	strcpy( info->compression, "JPG");	

	JPGDCloseDriver( jpgd, jpgdrv);

	Mfree( jpgd);
	Mfree( src);

	return ALL_OK;
}

