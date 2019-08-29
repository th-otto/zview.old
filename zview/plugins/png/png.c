#include "plugin.h"
#include "zvplugin.h"
#include <png.h>

#define VERSION 0x201
#define AUTHOR "Zorro,Thorsten Otto"

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long) ("PNG\0");
	}
	return -ENOSYS;
}
#endif

#define alpha_composite( composite, fg, alpha, bg) {									\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) +						\
                   ( uint16_t)( bg) * ( uint16_t)(255 - ( uint16_t)( alpha)) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);								\
}

struct _mypng_info {
	png_structp png_ptr;
	png_infop info_ptr;
	FILE *png_file;
	boolean first_pass;
	uint8_t *line_buffer;
	uint8_t *png_image1;
	uint8_t *png_image_ptr;
	int16_t number_passes;
	size_t input_rowbytes;
	png_byte bit_depth;   /* bit depth of row (1, 2, 4, or 8) */
	png_byte channels;    /* number of channels (1, 2, 3, or 4) */
	png_byte interlace_type;
	jmp_buf jmpbuf;
};



static void mypng_error_handler(png_structp png_ptr, png_const_charp msg)
{
	struct _mypng_info *myinfo;

#if 0
	printf("png error: %s\n", msg);
#endif
	(void) msg;
	
	/* This function, aside from the extra step of retrieving the "error
	 * pointer" (below) and the fact that it exists within the application
	 * rather than within libpng, is essentially identical to libpng's
	 * default error handler.  The second point is critical:  since both
	 * setjmp() and longjmp() are called from the same code, they are
	 * guaranteed to have compatible notions of how big a jmp_buf is,
	 * regardless of whether _BSD_SOURCE or anything else has (or has not)
	 * been defined. */

	myinfo = (struct _mypng_info *)png_get_error_ptr(png_ptr);
	if (myinfo == NULL)
	{									/* we are completely hosed now */
#if 0
		errout("writepng severe error:  jmpbuf not recoverable; terminating.\n");
		fflush(stderr);
#endif
		Pterm(-1);
	}

	/* Now we have our data structure we can use the information in it
	 * to return control to our own higher level code (all the points
	 * where 'setjmp' is called in this file.)  This will work with other
	 * error handling mechanisms as well - libpng always calls png_error
	 * when it can proceed no further, thus, so long as the error handler
	 * is intercepted, application code can do its own error recovery.
	 */
	longjmp(myinfo->jmpbuf, 1);
}


static void mypng_warning_handler(png_structp png_ptr, png_const_charp msg)
{
	/*
	 * Silently ignore any warning messages from libpng.
	 * They stupidly tend to introduce new warnings with every release,
	 * with the default warning handler writing to stdout and/or stderr,
	 * messing up the output of the CGI scripts.
	 */
	(void) png_ptr;
#if 0
	printf("png warning: %s\n", msg);
#endif
	(void) msg;
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
	FILE *png_file;
	char header[8];
	int header_size = sizeof(header);
#if 0
	png_color_16 my_background = { 0,0xFFFF,0xFFFF,0xFFFF,0xFFFF };
    png_color_16p image_background;
#endif
	png_textp png_text_ptr;
	int num_text = 0;
	struct _mypng_info *myinfo;

	if ((png_file = fopen(name, "rb")) == NULL)
		return FALSE;

	if (fread(header, 1, header_size, png_file) != header_size || png_sig_cmp(header, 0, header_size))
	{
		fclose(png_file);
		return FALSE;
	}
	if ((myinfo = calloc(1, sizeof(*myinfo))) == NULL)
	{
		fclose(png_file);
		return FALSE;
	}
	info->_priv_ptr = myinfo;
	myinfo->png_file = png_file;
	myinfo->first_pass = TRUE;
	myinfo->png_image1 = NULL;
	myinfo->line_buffer = NULL;
	myinfo->number_passes = 0;
	
	myinfo->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, myinfo, mypng_error_handler, mypng_warning_handler);

	if (!myinfo->png_ptr || (myinfo->info_ptr = png_create_info_struct(myinfo->png_ptr)) == NULL)
	{
		reader_quit(info);
		return FALSE;
	}

	if (setjmp(myinfo->jmpbuf))
	{
		reader_quit(info);
		return FALSE;
	}

	png_init_io(myinfo->png_ptr, myinfo->png_file);
	png_set_sig_bytes(myinfo->png_ptr, header_size);
	png_read_info(myinfo->png_ptr, myinfo->info_ptr);
	myinfo->channels = png_get_channels(myinfo->png_ptr, myinfo->info_ptr);
	myinfo->bit_depth = png_get_bit_depth(myinfo->png_ptr, myinfo->info_ptr);
	myinfo->interlace_type = png_get_interlace_type(myinfo->png_ptr, myinfo->info_ptr);

	if (png_get_color_type(myinfo->png_ptr, myinfo->info_ptr) == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(myinfo->png_ptr);
	if (png_get_color_type(myinfo->png_ptr, myinfo->info_ptr) == PNG_COLOR_TYPE_GRAY && myinfo->bit_depth < 8)
		png_set_expand(myinfo->png_ptr);
	if (png_get_valid(myinfo->png_ptr, myinfo->info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(myinfo->png_ptr);
#ifdef PNG_READ_16_TO_8_SUPPORTED
	if (myinfo->bit_depth == 16)
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
		png_set_scale_16(myinfo->png_ptr);
#else
		png_set_strip_16(myinfo->png_ptr);
#endif
#endif
	if (png_get_color_type(myinfo->png_ptr, myinfo->info_ptr) == PNG_COLOR_TYPE_GRAY ||
		png_get_color_type(myinfo->png_ptr, myinfo->info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(myinfo->png_ptr);

	strcpy(info->info, "Portable Network Format");
	strcpy(info->compression, "Defl");

	if (myinfo->interlace_type == PNG_INTERLACE_ADAM7)
	{
		strcat(info->info, " (Interlaced)");
		myinfo->number_passes = png_set_interlace_handling(myinfo->png_ptr);
	}

	png_read_update_info(myinfo->png_ptr, myinfo->info_ptr);

	png_get_text(myinfo->png_ptr, myinfo->info_ptr, &png_text_ptr, &num_text);

	myinfo->input_rowbytes = png_get_rowbytes(myinfo->png_ptr, myinfo->info_ptr);
	
	info->width = png_get_image_width(myinfo->png_ptr, myinfo->info_ptr);
	info->height = png_get_image_height(myinfo->png_ptr, myinfo->info_ptr);
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->components = myinfo->channels > 3 ? 3 : myinfo->channels;
	info->planes = png_get_bit_depth(myinfo->png_ptr, myinfo->info_ptr);
	info->colors = 1L << info->planes;
	info->delay = 0;
	info->orientation = UP_TO_DOWN;
	info->page = 1;
	info->num_comments = num_text;
	info->max_comments_length = 0;
	info->indexed_color = FALSE;

	if (myinfo->channels == 4 || myinfo->interlace_type)
	{
		myinfo->line_buffer = (uint8_t *) malloc(myinfo->input_rowbytes + 64);

		if (myinfo->line_buffer == NULL)
		{
			reader_quit(info);
			return FALSE;
		}
	}

	if (num_text)
	{
		int16_t i;
		uint16_t len;

		for (i = 0; i < num_text; i++)
		{
			len = (uint16_t)(strlen(png_text_ptr[i].key) + strlen(png_text_ptr[i].text) + 2);
			info->max_comments_length = MAX(info->max_comments_length, len);
		}
	}

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
	int16_t i;
	struct _mypng_info *myinfo = (struct _mypng_info *)info->_priv_ptr;
	png_textp png_text_ptr;
	int num_text;

	png_get_text(myinfo->png_ptr, myinfo->info_ptr, &png_text_ptr, &num_text);

	for (i = 0; i < txtdata->lines; i++)
		sprintf(txtdata->txt[i], "%s: %s", png_text_ptr[i].key, png_text_ptr[i].text);
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
	struct _mypng_info *myinfo = (struct _mypng_info *)info->_priv_ptr;
	uint8_t *buf;
	int16_t i;

	if (myinfo->channels == 4)
		buf = myinfo->line_buffer;
	else
		buf = buffer;

	if (setjmp(myinfo->jmpbuf))
		return FALSE;

	if (myinfo->interlace_type == PNG_INTERLACE_ADAM7)
	{
		/* We make the first pass here and not before to avoid memory fragmentation */
		if (myinfo->first_pass)
		{
			int16_t pass;
			int16_t y = info->height;

#ifndef PNG_READ_INTERLACING_SUPPORTED
			if (passes == PNG_INTERLACE_ADAM7_PASSES)
				y = PNG_PASS_ROWS(y, pass);
#endif

			png_start_read_image(myinfo->png_ptr);
			
			myinfo->png_image1 = (uint8_t *) malloc(myinfo->input_rowbytes * (info->height + 1));

			myinfo->png_image_ptr = myinfo->png_image1;

			for (pass = 0; pass < myinfo->number_passes; pass++)
			{
				for (i = 0; i < y; i++)
				{
					png_bytep row = myinfo->png_image1 + i * myinfo->input_rowbytes;
					png_read_row(myinfo->png_ptr, myinfo->line_buffer, row);
				}
			}
			myinfo->first_pass = FALSE;
		}

		memcpy(buf, myinfo->png_image_ptr, myinfo->input_rowbytes);
		myinfo->png_image_ptr += myinfo->input_rowbytes;
	} else
	{
		png_read_row(myinfo->png_ptr, buf, NULL);
	}

	if (myinfo->channels == 4)
	{
		uint8_t red, green, blue;
		uint8_t r, g, b, a;
		uint8_t *dest = buffer;
		uint8_t *buf_ptr = buf;

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
	struct _mypng_info *myinfo = (struct _mypng_info *)info->_priv_ptr;

	if (myinfo)
	{
		free(myinfo->png_image1);
		myinfo->png_image1 = NULL;
		
		free(myinfo->line_buffer);
		myinfo->line_buffer = NULL;
		
		if (setjmp(myinfo->jmpbuf) == 0)
			png_read_end(myinfo->png_ptr, myinfo->info_ptr);

		png_destroy_read_struct(&myinfo->png_ptr, &myinfo->info_ptr, NULL);
		if (myinfo->png_file)
		{
			fclose(myinfo->png_file);
			myinfo->png_file = NULL;
		}
		free(myinfo);
		info->_priv_ptr = NULL;
	}
}
