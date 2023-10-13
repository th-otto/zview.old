/*
 * webp.c - common code for SLB and LDG plugins
 *
 * Copyright (C) 2023 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright file.
 */

#include "plugin.h"
#include "zvplugin.h"
#include <webp/decode.h>
#include <webp/encode.h>
#include <webp/demux.h>
#include "zvwebp.h"

#define VERSION 0x100
#define NAME    "Image format for the Web"
#define AUTHOR  "Thorsten Otto"
#define DATE     __DATE__ " " __TIME__
#define MISCINFO "Using libwep version " WEBP_VERSION_STR

#define NF_DEBUG 0

#if NF_DEBUG

#define DEBUG_PREFIX "WebP: "

#include <mint/arch/nf_ops.h>

#ifdef PLUGIN_SLB

static long nfid_stderr;

#pragma GCC optimize "-fomit-frame-pointer"

static long __attribute__((noinline)) __CDECL _nf_get_id(const char *feature_name)
{
	register long ret __asm__ ("d0");
	(void)(feature_name);
	__asm__ volatile(
		"\t.word 0x7300\n"
	: "=g"(ret)  /* outputs */
	: /* inputs  */
	: __CLOBBER_RETURN("d0") "d1", "cc" AND_MEMORY /* clobbered regs */
	);
	return ret;
}


static long __attribute__((noinline)) __CDECL _nf_call(long id, ...)
{
	register long ret __asm__ ("d0");
	(void)(id);
	__asm__ volatile(
		"\t.word 0x7301\n"
	: "=g"(ret)  /* outputs */
	: /* inputs  */
	: __CLOBBER_RETURN("d0") "d1", "cc" AND_MEMORY /* clobbered regs */
	);
	return ret;
}

/*
 * call the functions indirectly, so the unused parameters above
 * are not optimized away
 */
static struct {
	long __CDECL (*nf_get_id)(const char *feature_name);
	long __CDECL (*nf_call)(long id, ...);
} ops = {
	_nf_get_id,
	_nf_call
};


__attribute__((format(__printf__, 1, 0)))
int nf_debugvprintf(const char *format, va_list args)
{
	static char buf[2048];
	int ret;

	if (nfid_stderr == 0)
		nfid_stderr = ops.nf_get_id("NF_STDERR");
	ret = vsnprintf(buf, sizeof(buf), format, args);
	ret = (int)ops.nf_call(nfid_stderr | 0, (uint32_t)buf);
	return ret;
}

__attribute__((format(__printf__, 1, 2)))
int nf_debugprintf(const char *format, ...)
{
	int ret;
	va_list args;

	va_start(args, format);
	ret = nf_debugvprintf(format, args);
	va_end(args);
	return ret;
}

#endif

#define nf_debugprint(x) nf_debugprintf x

#else

#define nf_debugprint(x)

#endif


uint16_t compression_level = 6;
int quality = 75;

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	nf_debugprint((DEBUG_PREFIX "get_option: %d\n", which));
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE | CAN_ENCODE;
	case OPTION_EXTENSIONS:
		return (long) ("WEB\0" "WEBP\0");

	case OPTION_QUALITY:
		return quality;
	case OPTION_COMPRESSION:
		return compression_level;

	case INFO_NAME:
		return (long)NAME;
	case INFO_VERSION:
		return VERSION;
	case INFO_DATETIME:
		return (long)DATE;
	case INFO_AUTHOR:
		return (long)AUTHOR;
	case INFO_MISC:
		return (long)MISCINFO;
	}
	return -ENOSYS;
}

long __CDECL set_option(zv_int_t which, zv_int_t value)
{
	nf_debugprint((DEBUG_PREFIX "set_option: %d\n", which));
	switch (which)
	{
	case OPTION_CAPABILITIES:
	case OPTION_EXTENSIONS:
		return -EACCES;
	case OPTION_QUALITY:
		if (value < 0 || value > 100)
			return -ERANGE;
		quality = value;
		return value;
	case OPTION_COMPRESSION:
		compression_level = value;
		return value;
	}
	return -ENOSYS;
}

#ifdef WEBP_SLB

static long init_webp_slb(void)
{
	struct _zview_plugin_funcs *funcs;
	SLB *slb;
	long ret;

	funcs = get_slb_funcs();
	slb = get_slb_funcs()->p_slb_get(LIB_WEBP);
	if (slb->handle == 0)
	{
		if ((ret = funcs->p_slb_open(LIB_WEBP)) < 0)
			return ret;
	}
	return 0;
}
#endif

#endif


#define alpha_composite(composite, fg, alpha, bg) {									\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) +						\
                   ( uint16_t)( bg) * ( uint16_t)(255 - ( uint16_t)( alpha)) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);								\
}

struct _mywebp_info {
	VP8StatusCode status;
	WebPData input_data;
	FILE *webp_file;
	uint32_t feature_flags;
	int32_t canvas_width;
	int32_t canvas_height;
	int loop_count;
	uint32_t bgcolor;
	int32_t num_frames;
	uint8_t *bmap;
	uint8_t channels;
	size_t stride_width;
	size_t row_width;
};

static uint32_t filesize(int16_t fd)
{
	uint32_t size;

	size = Fseek(0, fd, SEEK_END);
	Fseek(0, fd, SEEK_SET);
	return size;
}


/*==================================================================================*
 * boolean __CDECL reader_init:														*
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
boolean __CDECL reader_init(const char *name, IMGINFO info)
{
	struct _mywebp_info *myinfo;
	uint32_t file_length;
	int16_t handle;
	WebPDemuxer *demux;
	size_t bitmap_size;
	WebPChunkIterator chunk_iter;

	nf_debugprint((DEBUG_PREFIX "reader_init: %p %s\n", (void *)info, name));

#ifdef PLUGIN_SLB
#ifdef WEBP_SLB
	if (init_webp_slb() < 0)
	{
		nf_debugprint((DEBUG_PREFIX "init_webp_slb() failed\n"));
		return FALSE;
	}
#endif
#endif

	if ((handle = (int16_t) Fopen(name, 0)) < 0)
	{
		nf_debugprint((DEBUG_PREFIX "fopen() failed\n"));
		return FALSE;
	}

	file_length = filesize(handle);

	if ((myinfo = malloc(sizeof(*myinfo) + file_length + 1)) == NULL)
	{
		nf_debugprint((DEBUG_PREFIX "malloc() failed\n"));
		Fclose(handle);
		return FALSE;
	}
	memset(myinfo, 0, sizeof(*myinfo));
	myinfo->input_data.bytes = (uint8_t *)myinfo + sizeof(*myinfo);
	myinfo->input_data.size = file_length;
	info->_priv_ptr = myinfo;

	if ((uint32_t) Fread(handle, file_length, myinfo->input_data.bytes) != file_length)
	{
		nf_debugprint((DEBUG_PREFIX "fread() failed\n"));
		Fclose(handle);
		reader_quit(info);
		return FALSE;
	}
	Fclose(handle);
	demux = WebPDemux(&myinfo->input_data);
	if (demux == NULL)
	{
		nf_debugprint((DEBUG_PREFIX "malloc() failed\n"));
		reader_quit(info);
		return FALSE;
	}

	myinfo->canvas_width = WebPDemuxGetI(demux, WEBP_FF_CANVAS_WIDTH);
	myinfo->canvas_height = WebPDemuxGetI(demux, WEBP_FF_CANVAS_HEIGHT);
	/* check for resonable limits */
	if (myinfo->canvas_width <= 0 ||
		myinfo->canvas_width >= 32000 ||
		myinfo->canvas_height <= 0 ||
		myinfo->canvas_height >= 32000)
	{
		nf_debugprint((DEBUG_PREFIX "image too large: %ldx%ld\n", (long)myinfo->canvas_width, (long)myinfo->canvas_height));
		reader_quit(info);
	}
	myinfo->feature_flags = WebPDemuxGetI(demux, WEBP_FF_FORMAT_FLAGS);
	myinfo->loop_count = WebPDemuxGetI(demux, WEBP_FF_LOOP_COUNT);
	myinfo->bgcolor = WebPDemuxGetI(demux, WEBP_FF_BACKGROUND_COLOR);
	myinfo->num_frames = WebPDemuxGetI(demux, WEBP_FF_FRAME_COUNT);
	nf_debugprint((DEBUG_PREFIX "width: %u\n", myinfo->canvas_width));
	nf_debugprint((DEBUG_PREFIX "height: %u\n", myinfo->canvas_height));
	nf_debugprint((DEBUG_PREFIX "frames: %u\n", myinfo->num_frames));

	myinfo->channels = myinfo->feature_flags & ALPHA_FLAG ? 4 : 3;
	myinfo->stride_width = myinfo->canvas_width * myinfo->channels;
	myinfo->row_width = myinfo->canvas_width * 3;
	bitmap_size = myinfo->stride_width * myinfo->canvas_height;
	myinfo->bmap = WebPMalloc(bitmap_size);
	if (myinfo->bmap == NULL)
	{
		nf_debugprint((DEBUG_PREFIX "malloc() failed\n"));
		WebPDemuxDelete(demux);
		reader_quit(info);
		return FALSE;
	}

	strcpy(info->info, "WebP image format");
	info->width = myinfo->canvas_width;
	info->height = myinfo->canvas_height;
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->components = myinfo->channels > 3 ? 3 : myinfo->channels;
	info->planes = myinfo->channels * 8;
	info->colors = 1L << MIN(info->planes, 24);
	info->page = 1;
	info->delay = 0;
	info->num_comments = 0;
	info->max_comments_length = 0;
	info->indexed_color = FALSE;
	info->orientation = UP_TO_DOWN;

	/* Attention: compression field is only 5 chars */
	strcpy(info->compression, "Unkn");
	if (WebPDemuxGetChunk(demux, "VP8X", 1, &chunk_iter))
	{
		WebPDemuxReleaseChunkIterator(&chunk_iter);
		strcpy(info->compression, "VP8X");
	} else if (WebPDemuxGetChunk(demux, "VP8L", 1, &chunk_iter))
	{
		WebPDemuxReleaseChunkIterator(&chunk_iter);
		strcpy(info->compression, "VP8L");
		strcat(info->info, " (lossless)");
	} else if (WebPDemuxGetChunk(demux, "VP8", 1, &chunk_iter))
	{
		WebPDemuxReleaseChunkIterator(&chunk_iter);
		strcpy(info->compression, "VP8");
	}

	WebPDecodeRGBInto(myinfo->input_data.bytes, myinfo->input_data.size, myinfo->bmap, bitmap_size, myinfo->row_width);
	WebPDemuxDelete(demux);

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function mus be always present.					*
 *		It fills txtdata struct. with the text present in the picture ( if any).	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		txtdata		->	The destination text buffer.								*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_get_txt(IMGINFO info, txt_data *txtdata)
{
	nf_debugprint((DEBUG_PREFIX "reader_get_txt\n"));
	(void)info;
	(void)txtdata;
}


/*==================================================================================*
 * boolean __CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_read(IMGINFO info, uint8_t *buffer)
{
	struct _mywebp_info *myinfo = (struct _mywebp_info *)info->_priv_ptr;
	int16_t i;

#define reader_cur_row(info) ((info)->_priv_var_more)

	if (reader_cur_row(info) < myinfo->canvas_height)
	{
		uint8_t *src = myinfo->bmap + reader_cur_row(info) * myinfo->stride_width;
		
		if (myinfo->channels == 4)
		{
			uint8_t red, green, blue;
			uint8_t r, g, b, a;
			uint8_t *dest = buffer;
			uint8_t *buf_ptr = src;
	
			for (i = info->width; i > 0; --i)
			{
				r = *buf_ptr++;
				g = *buf_ptr++;
				b = *buf_ptr++;
				a = *buf_ptr++;
	
				if (a == 255)
				{
					red = r;
					green = g;
					blue = b;
				} else if (a == 0)
				{
					red = 0xFF;
					green = 0xFF;
					blue = 0xFF;
				} else
				{
					uint32_t transparent_color = info->background_color;
	
					alpha_composite(red, r, a, (transparent_color >> 16) & 0xFF);
					alpha_composite(green, g, a, (transparent_color >> 8) & 0xFF);
					alpha_composite(blue, b, a, (transparent_color) & 0xFF);
				}

				*dest++ = red;
				*dest++ = green;
				*dest++ = blue;
			}
		} else
		{
			memcpy(buffer, src, myinfo->row_width);
		}
		reader_cur_row(info)++;
	}
	return TRUE;
}

/*==================================================================================*
 * boolean __CDECL reader_quit:														*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_quit(IMGINFO info)
{
	struct _mywebp_info *myinfo = (struct _mywebp_info *)info->_priv_ptr;

	nf_debugprint((DEBUG_PREFIX "reader_quit\n"));
	if (myinfo)
	{
		WebPFree(myinfo->bmap);
		free(myinfo);
		info->_priv_ptr = NULL;
	}
}



/*==================================================================================*
 * boolean __CDECL encoder_init:													*
 *		Open the file "name", fit the "info" struct. ( see zview.h) and make others	*
 *		things needed by the encoder.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name		->	The file to open.											*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL encoder_init(const char *name, IMGINFO info)
{
	(void)(name);

	info->planes   			= 24;
	info->colors  			= 16777215L;
	info->orientation		= UP_TO_DOWN;
	info->memory_alloc 		= TT_RAM;
	info->indexed_color	 	= FALSE;
	info->page			 	= 1;
	info->delay 		 	= 0;
	info->_priv_ptr	 		= 0;
	info->_priv_ptr_more	= NULL;
	info->__priv_ptr_more	= NULL;
	info->_priv_var	 		= 0;
	info->_priv_var_more	= 0;

	return TRUE;
}


/*==================================================================================*
 * boolean encoder_write:															*
 *		This function write data from buffer to file								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The source buffer.											*
 *		info		->	The IMGINFO struct		.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL encoder_write(IMGINFO info, uint8_t *buffer)
{
	(void) info;
	(void) buffer;
	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL encoder_quit:													*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL encoder_quit(IMGINFO info)
{
	struct _mywebp_info *myinfo = (struct _mywebp_info *)info->_priv_ptr;

	if (myinfo)
	{
		if (myinfo->webp_file)
		{
			fclose(myinfo->webp_file);
			myinfo->webp_file = NULL;
		}
		free(myinfo);
		info->_priv_ptr = NULL;
	}
}
