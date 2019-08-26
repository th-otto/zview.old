/*
   notes: 16-bit grayscale might appear reversed, not sure test files are right
       32-bit color mapped had wrong background color, bad test file?
         imagecopy looked the same

   1.00 initial release
   1.01 fixed a buffer overrun
   1.02 added debug information via alt key
   1.03 added decompression overrun test, show file name in debug
   1.04 uses half the ram it did before, converts directly from decomp buffer
   1.05 improved 15/16-bit conversions
   1.06 fixed save (id size err, caused colors to be off), added exif support
   1.07 firebee fixes (patched purec library)
   1.08 fixed info->_priv_ptr assignment
   1.09 optimized intel i/o
   1.10 made arrays global, better error handling, faster rle decoding,
        added 16-bit grayscale support, illegal chars removed from exif,
        fixed encoder conflict
   1.11 add more debug info
   1.12 Code cleanup, conversion to SLB
 */

#include "plugin.h"
#include "zvplugin.h"
#include "libtarga.h"

#define VERSION 0x112
#define AUTHOR "Zorro"

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long) ("TGA\0");
	}
	return -ENOSYS;
}
#endif

#define alpha_composite( composite, fg, alpha) {						\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);				\
}

#define DEBUG 1

#include "depack.c"

#if DEBUG
#define print(x) if (pflg) printf x
#else
#define print(x)
#endif

/*==================================================================================*
 * void tga_write_pixel_to_mem:														*
 *		Write the pixel to the data regarding how the header says the data 			*
 *		is ordered.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		dst				->	the pixel is wrote here.								*
 *		orientation 	->	the pixel orientation ( from TGA header).				*
 *		pixel_position	->	the pixel position in the x axis.						*
 *		width			->	the image's width.										*
 *		pixel			->	the 24 bits pixel to write.								*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
static inline void tga_write_pixel_to_mem(uint8_t *dst, uint8_t orientation, uint16_t pixel_position, uint16_t width, uint8_t *rgb)
{
	uint16_t x;
	uint16_t addy;

	switch (orientation)
	{
	case TGA_UPPER_LEFT:
	case TGA_LOWER_LEFT:
	default:
		x = pixel_position % width;
		break;

	case TGA_LOWER_RIGHT:
	case TGA_UPPER_RIGHT:
		x = width - 1 - (pixel_position % width);
		break;
	}

	addy = x * 3;

	dst[addy++] = rgb[0];
	dst[addy++] = rgb[1];
	dst[addy] = rgb[2];
}


/*==================================================================================*
 * uint32_t tga_convert_color:														*
 *		Write the pixel to the data regarding how the header says the data 			*
 *		is ordered.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		rgb				->	bgra value.												*
 *		bpp_in			->	original image bitplanes.								*
 *		alphabits		->	Alpha bit... 1 if present else 0.						*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      the converted pixel.														*
 *==================================================================================*/
static inline void tga_convert_color(uint8_t *bgra, uint8_t *rgb, uint8_t bpp_in, uint8_t alphabits)
{
	uint8_t r = 0,
		g = 0,
		b = 0,
		a = 0;

	switch (bpp_in)
	{
	case 32:
	case 24:
		if (alphabits)
		{
			/* not premultiplied alpha -- multiply. */
			a = bgra[3];
			alpha_composite(r, bgra[2], a);
			alpha_composite(g, bgra[1], a);
			alpha_composite(b, bgra[0], a);
		} else
		{
			r = bgra[2];
			g = bgra[1];
			b = bgra[0];
		}
		break;

	case 16:
	case 15:
		{
			/* 16-bit to 32-bit; (force alpha to full) */
			uint16_t src16 = (((uint16_t) bgra[1] << 8) | bgra[0]);

			b = ((src16) & 0x001F) << 3;
			g = ((src16 >> 5) & 0x001F) << 3;
			r = ((src16 >> 10) & 0x001F) << 3;
		}
		break;
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
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
	int16_t i;
	uint32_t num_pixels;
	uint32_t file_length;
	int16_t handle;
	uint8_t header_buf[HDR_LENGTH];
	tga_pic *tga_struct;
	uint32_t bitmap_size;
	uint8_t pixel[4];

#if DEBUG
	int pflg = (Kbshift(-1) & K_ALT) != 0;				/* alternate key */
	print(("\n"));
	print(("%s\n", name));
#endif

	if ((handle = (int16_t) Fopen(name, 0)) < 0)
	{
		print(("fopen() failed\n"));
		return FALSE;
	}

	if (Fread(handle, HDR_LENGTH, &header_buf) != HDR_LENGTH)
	{
		Fclose(handle);
		return FALSE;
	}

	tga_struct = (tga_pic *) malloc(sizeof(tga_pic));

	if (tga_struct == NULL)
	{
		Fclose(handle);
		return FALSE;
	}

	/* byte order is important here. */
	tga_struct->tga.idlen = header_buf[HDR_IDLEN];
	tga_struct->tga.cmap_type = header_buf[HDR_CMAP_TYPE];
	tga_struct->tga.image_type = header_buf[HDR_IMAGE_TYPE];
	tga_struct->tga.cmap_first = (uint16_t) header_buf[HDR_CMAP_FIRST + 0] + ((uint16_t) header_buf[HDR_CMAP_FIRST + 1] << 8);
	tga_struct->tga.cmap_length = (uint16_t) header_buf[HDR_CMAP_LENGTH + 0] + ((uint16_t) header_buf[HDR_CMAP_LENGTH + 1] << 8);
	tga_struct->tga.cmap_entry_size = header_buf[HDR_CMAP_ENTRY_SIZE];
	tga_struct->tga.img_spec_xorig = (uint16_t) header_buf[HDR_IMG_SPEC_XORIGIN + 0] + ((uint16_t) header_buf[HDR_IMG_SPEC_XORIGIN + 1] << 8);
	tga_struct->tga.img_spec_yorig = (uint16_t) header_buf[HDR_IMG_SPEC_YORIGIN + 0] + ((uint16_t) header_buf[HDR_IMG_SPEC_YORIGIN + 1] << 8);
	tga_struct->tga.img_spec_width = (uint16_t) header_buf[HDR_IMG_SPEC_WIDTH + 0] + ((uint16_t) header_buf[HDR_IMG_SPEC_WIDTH + 1] << 8);
	tga_struct->tga.img_spec_height = (uint16_t) header_buf[HDR_IMG_SPEC_HEIGHT + 0] + ((uint16_t) header_buf[HDR_IMG_SPEC_HEIGHT + 1] << 8);
	tga_struct->tga.img_spec_pix_depth = header_buf[HDR_IMG_SPEC_PIX_DEPTH];
	tga_struct->tga.img_spec_img_desc = header_buf[HDR_IMG_SPEC_IMG_DESC];

	num_pixels = tga_struct->tga.img_spec_width * tga_struct->tga.img_spec_height;

	if (num_pixels == 0)
	{
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}

	print(("cmb=%i\n", tga_struct->tga.cmap_entry_size));

	file_length = Fseek(0L, handle, 2);

	tga_struct->alphabits = tga_struct->tga.img_spec_img_desc & 0x0F;
	tga_struct->handle = handle;

	/* read the image id, if there is one */
	if (tga_struct->tga.idlen)
	{									/* SKIP IDENTIFICATION DATA? */
		print(("has image id\n"));
		if (Fread(handle, (uint32_t) tga_struct->tga.idlen, tga_struct->id) != tga_struct->tga.idlen)
		{
			free(tga_struct);
			Fclose(handle);
			return FALSE;
		}
	}
	tga_struct->id[tga_struct->tga.idlen] = 0;	/* add missing null */

	if (tga_struct->tga.cmap_type == 0)
	{									/* no color map? */
		print(("no color map\n"));
		if (tga_struct->tga.img_spec_pix_depth == 8)
		{								/* 256 colors? */
			for (i = 0; i < 256; i++)
			{
				info->palette[i].red = info->palette[i].green = info->palette[i].blue = i;
			}
		}
	} else if (tga_struct->tga.cmap_type == 1)
	{									/* color map type? */
		print(("has color map\n"));
		if (tga_struct->tga.cmap_entry_size == 16)
		{
			print(("color map 16-bit\n"));
			for (i = tga_struct->tga.cmap_first; i < tga_struct->tga.cmap_first + tga_struct->tga.cmap_length; i++)
			{
				Fread(handle, 2, pixel);	/* rgb555, intel order */
				info->palette[i].red = ((pixel[1] >> 2) & 0x1f) << 3;
				info->palette[i].green = (((pixel[0] >> 5) & 0x07) | ((pixel[1] << 3) & 0x18)) << 3;
				info->palette[i].blue = (pixel[0] & 0x1f) << 3;
			}
		} else if (tga_struct->tga.cmap_entry_size == 24)
		{
			print(("color map 24-bit\n"));
			for (i = tga_struct->tga.cmap_first; i < tga_struct->tga.cmap_first + tga_struct->tga.cmap_length; i++)
			{
				Fread(handle, 3, pixel);	/* bgr */
				info->palette[i].red = pixel[2];
				info->palette[i].green = pixel[1];
				info->palette[i].blue = pixel[0];
			}
		} else if (tga_struct->tga.cmap_entry_size == 32)
		{
			print(("color map 32-bit\n"));
			for (i = tga_struct->tga.cmap_first; i < tga_struct->tga.cmap_first + tga_struct->tga.cmap_length; i++)
			{
				Fread(handle, 4, pixel);	/* bgra */
				info->palette[i].red = pixel[2];
				info->palette[i].green = pixel[1];
				info->palette[i].blue = pixel[0];
			}
		} else
		{
			print(("invalid color map depth\n"));
			free(tga_struct);
			Fclose(handle);
			return FALSE;
		}
	} else
	{									/* vendor specific, not supported */
#if 0
		cs = (tga_struct->tga.cmap_entry_size / 8) * tga_struct->tga.cmap_length;  /* calc color map size */
		Fseek(cs, handle, SEEK_CUR);   /* try and skip it */
#endif
		print(("unsupported color map type\n"));
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}

	/*                   01234567890123456789012345  */
	strcpy(info->info, "Truevision Targa (Type _) ");

	switch (tga_struct->tga.image_type)
	{
	case TGA_IMG_UNC_PALETTED:
		info->info[23] = '1';
		break;
	case TGA_IMG_UNC_TRUECOLOR:
		info->info[23] = '2';
		break;
	case TGA_IMG_UNC_GRAYSCALE:
		info->info[23] = '3';
		break;
	case TGA_IMG_RLE_PALETTED:
		info->info[23] = '9';
		break;
	case TGA_IMG_RLE_TRUECOLOR:
		info->info[23] = '1';
		info->info[24] = '0';
		info->info[25] = ')';
		break;
	case TGA_IMG_RLE_GRAYSCALE:
		info->info[23] = '1';
		info->info[24] = '1';
		info->info[25] = ')';
		break;
	default:
		print(("invalid image type\n"));
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}

	/* compute number of bytes in an image data unit (either index or BGR triple) */
	if (tga_struct->tga.img_spec_pix_depth < 8 || tga_struct->tga.img_spec_pix_depth > 32)
	{
		print(("invalid pixel depth\n"));
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}
	if (tga_struct->tga.img_spec_pix_depth == 15)
	{									/* fix */
		tga_struct->tga.img_spec_pix_depth = 16;
		print(("pixel depth 15-bit\n"));
	}
	if (tga_struct->tga.cmap_entry_size == 15)
	{									/* fix */
		tga_struct->tga.cmap_entry_size = 16;
		print(("color map depth 15-bit\n"));
	}
	tga_struct->bytes_per_pix = (tga_struct->tga.img_spec_pix_depth + 7) >> 3;

	/* assume that there's one byte per pixel */
	if (tga_struct->bytes_per_pix == 0)
		tga_struct->bytes_per_pix = 1;

	tga_struct->line_size = tga_struct->tga.img_spec_width * tga_struct->bytes_per_pix;
	tga_struct->img_buf_len = tga_struct->line_size << 1;

	tga_struct->img_buf = (uint8_t *) malloc(tga_struct->img_buf_len + 256L);

	if (tga_struct->img_buf == NULL)
	{
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}

	bitmap_size = (tga_struct->tga.img_spec_width * (uint32_t)tga_struct->bytes_per_pix) * tga_struct->tga.img_spec_height;				/* CALC BIT MAP SIZE IN BYTES */
	print(("bytes_per_pix=%u\n", tga_struct->bytes_per_pix));

	tga_struct->orientation = (tga_struct->tga.img_spec_img_desc & 0x30) >> 4;
	tga_struct->rest_length = file_length - (tga_struct->tga.idlen + HDR_LENGTH);

	if (tga_struct->img_buf_len > tga_struct->rest_length)
		tga_struct->img_buf_len = tga_struct->rest_length;

	tga_struct->img_buf_offset = 0;
	tga_struct->img_buf_used = Fread(handle, tga_struct->img_buf_len, tga_struct->img_buf);
	tga_struct->rest_length -= tga_struct->img_buf_used;

	tga_struct->bmap = malloc(bitmap_size + 256L);			/* little extra just in case */
	if (tga_struct->bmap == NULL)
	{
		print(("malloc(bitmap) failed\n"));
		free(tga_struct->img_buf);
		free(tga_struct);
		Fclose(handle);
		return FALSE;
	}

	info->width = tga_struct->tga.img_spec_width;
	info->height = tga_struct->tga.img_spec_height;
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->components = TGA_TRUECOLOR_24;
	info->planes = MIN(tga_struct->tga.img_spec_pix_depth, 24);
	info->colors = 1L << info->planes;
	info->page = 1;
	info->delay = 0;
	info->num_comments = 0;
	info->max_comments_length = 0;
	info->indexed_color = FALSE;
	info->_priv_ptr = (void *) tga_struct;

	switch ((tga_struct->tga.img_spec_img_desc >> 4) & 3)
	{									/* determine orientation */
	case TGA_LOWER_LEFT:
	case TGA_LOWER_RIGHT:
		info->orientation = DOWN_TO_UP;
		print(("orientation flipped\n"));
		break;
	case TGA_UPPER_LEFT:
	case TGA_UPPER_RIGHT:
		info->orientation = UP_TO_DOWN;
		print(("orientation normal\n"));
		break;
	}

	/* unpacked as is, pixel data is not corrected for motorola */
	if (tga_struct->tga.image_type == TGA_IMG_RLE_PALETTED ||
		tga_struct->tga.image_type == TGA_IMG_RLE_TRUECOLOR ||
		tga_struct->tga.image_type == TGA_IMG_RLE_GRAYSCALE)
	{									/* compressed? */
		uint8_t *temp;
		uint32_t m;

		print(("rle compressed\n"));
		temp = malloc(file_length);		/* extra with header */
		if (temp == NULL)
		{
			free(tga_struct->bmap);
			free(tga_struct->img_buf);
			free(tga_struct);
			Fclose(handle);
			return FALSE;
		}
		Fread(handle, file_length - HDR_LENGTH - strlen(tga_struct->id), temp);
		m = decode_tga(temp, tga_struct->bmap, bitmap_size, tga_struct->bytes_per_pix);
		if (m != bitmap_size)
		{
			print(("decompress overrun!\n"));
		}
		free(temp);
		strcpy(info->compression, "RLE");
	} else
	{									/* uncompressed? */
		print(("no compression\n"));
		Fread(handle, bitmap_size, tga_struct->bmap);
		strcpy(info->compression, "None");
	}

	Fclose(handle);

	if (tga_struct->tga.image_type == TGA_IMG_UNC_PALETTED || tga_struct->tga.image_type == TGA_IMG_RLE_PALETTED)
	{
		print(("type 1/9 8-bit color mapped\n"));
		tga_struct->tga.image_type = 9;
		info->indexed_color = TRUE;
	} else if (tga_struct->tga.image_type == TGA_IMG_UNC_TRUECOLOR || tga_struct->tga.image_type == TGA_IMG_RLE_TRUECOLOR)
	{
		print(("type 2/10 16/24/32-bit high/true color\n"));
		tga_struct->tga.image_type = tga_struct->tga.img_spec_pix_depth;
		info->indexed_color = FALSE;
	} else if (tga_struct->tga.image_type == TGA_IMG_UNC_GRAYSCALE || tga_struct->tga.image_type == TGA_IMG_RLE_GRAYSCALE)
	{
		print(("type 3/11 8/16-bit mono/grayscale\n"));
		if (tga_struct->bytes_per_pix == 1)
		{
			tga_struct->tga.image_type = 11;
		} else if (tga_struct->bytes_per_pix == 2)
		{
			tga_struct->tga.image_type = 22;
		}
		info->indexed_color = FALSE;
	}

	print(("type=%i\n", tga_struct->tga.image_type));

	if (tga_struct->id[0])
	{
		info->num_comments = 1;
		info->max_comments_length = (uint16_t) strlen(tga_struct->id);
	}

	tga_struct->cur_pos = 0;						/* reset - y pos */

	return TRUE;
}


/*==================================================================================*
 * int16_t fill_img_buf:															*
 *		fill the buffer "tga_struct->img_buf" with a line from file.	 			*
 *		the buffer can contain 2 lines, this function search also where write the	*
 *		new line from the file.														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct		->	tga_pic struct. with all the wanted information.		*
 *		nread		 	->	internal counter.										*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      Always 1 ( need to code something more secure)								*
 *==================================================================================*/
static int16_t fill_img_buf(tga_pic *tga_struct, int32_t nread)
{
	tga_struct->img_buf_used -= nread;
	tga_struct->img_buf_offset += nread;

	if (tga_struct->img_buf_offset >= (tga_struct->img_buf_len >> 1))
	{
		if (tga_struct->img_buf_used > 0)
			memcpy(tga_struct->img_buf, tga_struct->img_buf + tga_struct->img_buf_offset, tga_struct->img_buf_used);

		nread = tga_struct->img_buf_offset;

		if (nread > tga_struct->rest_length)
			nread = tga_struct->rest_length;

		nread = Fread(tga_struct->handle, nread, tga_struct->img_buf + tga_struct->img_buf_used);

		tga_struct->img_buf_used += nread;
		tga_struct->img_buf_offset = 0;
		tga_struct->rest_length -= nread;
	}

	return TRUE;
}


/*==================================================================================*
 * uint8_t *unpack_line1:															*
 *		depack ( for RLE image) and copy data from source to destination. 			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct	->	tga_pic struct. with all the wanted information.			*
 *		src		 	->	the source buffer.											*
 *		dst		 	->	the destination buffer.										*
 *		line_width	->	the image width.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      Howmany source buffer position after that the job is done.					*
 *==================================================================================*/
static uint8_t *unpack_line1(tga_pic *tga_struct, uint8_t *src, uint8_t *dst, uint16_t line_width)
{
	uint16_t i, j;
	uint16_t bytes_per_pix = (int16_t) tga_struct->bytes_per_pix;
	uint8_t bgra[4];
	uint8_t rgb[3];
	uint8_t packet_header;
	uint8_t repcount;

	if (tga_struct->tga.image_type == TGA_IMG_UNC_TRUECOLOR)
	{
		for (i = 0; i < line_width; i++)
		{
			for (j = 0; j < bytes_per_pix; j++)
				bgra[j] = *src++;

			tga_convert_color(&bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);
			tga_write_pixel_to_mem(dst, tga_struct->orientation, i, line_width, &rgb[0]);
		}
	} else
	{
		uint16_t l;

		for (i = 0; i < line_width;)
		{
			packet_header = *src++;

			if (packet_header & 0x80)
			{
				for (j = 0; j < bytes_per_pix; j++)
					bgra[j] = *src++;

				tga_convert_color(&bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);

				repcount = (packet_header & 0x7F) + 1;

				/* write all the data out */
				for (j = 0; j < repcount; j++)
					tga_write_pixel_to_mem(dst, tga_struct->orientation, i + j, line_width, &rgb[0]);

				i += repcount;
			} else
			{
				repcount = (packet_header & 0x7F) + 1;

				for (l = 0; l < repcount; l++)
				{
					for (j = 0; j < bytes_per_pix; j++)
						bgra[j] = *src++;

					tga_convert_color(&bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);

					tga_write_pixel_to_mem(dst, tga_struct->orientation, i + l, line_width, &rgb[0]);
				}

				i += repcount;
			}
		}
	}

	return src;
}


/*==================================================================================*
 * int16_t unpack_line:																*
 *		Call the "source's buffer to destination's buffer" and						* 
 *		"image to source's buffer" functions. 										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct	->	tga_pic struct. with all the wanted information.			*
 *		dst		 	->	the destination buffer.										*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      1 if ok else 0.																*
 *==================================================================================*/
static int16_t unpack_line(tga_pic *tga_struct, uint8_t *dst)
{
	uint8_t *line_begin;
	int32_t nread;

	line_begin = tga_struct->img_buf + tga_struct->img_buf_offset;

	nread =
		(int32_t) (unpack_line1(tga_struct, line_begin, dst, (uint16_t) tga_struct->tga.img_spec_width) - line_begin);

	if (fill_img_buf(tga_struct, nread))
		return TRUE;
	return FALSE;
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
	tga_pic *tga_struct = (tga_pic *) info->_priv_ptr;
	uint8_t *bmap = tga_struct->bmap + tga_struct->cur_pos;
	uint8_t byt;
	uint16_t x;
	uint16_t rgb;

	switch (tga_struct->tga.image_type)
	{
	case 9:
		memcpy(buffer, bmap, info->width);
		tga_struct->cur_pos += info->width;
		break;
	case 11:
		tga_struct->cur_pos += info->width;
		for (x = 0; x < info->width; x++)
		{
			byt = *bmap++;
			*buffer++ = byt;
			*buffer++ = byt;
			*buffer++ = byt;
		}
		break;
	case 16:							/* arrrrrgggggbbbbb */
		tga_struct->cur_pos += (uint32_t)info->width << 1;
		for (x = 0; x < info->width; x++)
		{
			rgb = *bmap++;
			rgb |= *bmap++ << 8;
			*buffer++ = ((rgb >> 10) & 31) << 3;
			*buffer++ = ((rgb >> 5) & 31) << 3;
			*buffer++ = (rgb & 31) << 3;
		}
		break;
	case 22:							/* special case - gray 16-bit - aaaaaaaagggggggg */
		tga_struct->cur_pos += (uint32_t)info->width << 1;
		for (x = 0; x < info->width; x++)
		{
#if 0
			rgb = *bmap++;
			rgb = rgb | (*bmap++ << 8);
			rgb = rgb >> 8;              /* div rgb,256 */
#endif
			bmap++;				/* skip alpha */
			byt = 255 - *bmap++;	/* flipped to match osx preview */
			*buffer++ = byt;
			*buffer++ = byt;
			*buffer++ = byt;
		}
		break;
	case 24:							/* bbbbbbbbggggggggrrrrrrrr */
		tga_struct->cur_pos += (uint32_t)info->width * 3;
		for (x = 0; x < info->width; x++)
		{
			*buffer++ = bmap[2];
			*buffer++ = bmap[1];
			*buffer++ = bmap[0];
			bmap += 3;
		}
		break;
	case 32:							/* bbbbbbbbggggggggrrrrrrrraaaaaaaa */
		tga_struct->cur_pos += (uint32_t)info->width << 2;
		if (tga_struct->alphabits)
		{
			for (x = 0; x < info->width; x++)
			{
				uint8_t b = *bmap++;
				uint8_t g = *bmap++;
				uint8_t r = *bmap++;
				uint8_t a = *bmap++;
				alpha_composite(*buffer++, r, a);
				alpha_composite(*buffer++, g, a);
				alpha_composite(*buffer++, b, a);
			}
		} else
		{
			for (x = 0; x < info->width; x++)
			{
				*buffer++ = bmap[2];
				*buffer++ = bmap[1];
				*buffer++ = bmap[0];
				bmap += 4;
			}
		}
		break;
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
	tga_pic *tga_struct = (tga_pic *) info->_priv_ptr;

	if (info->num_comments)
	{
		int i;

		for (i = 0; tga_struct->id[i] != '\0'; i++)
		{
			if (tga_struct->id[i] < 32)
			{
				tga_struct->id[i] = 32;
			}
		}
		strcpy(txtdata->txt[0], tga_struct->id);
	}
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
	tga_pic *tga_struct = (tga_pic *) info->_priv_ptr;

	if (tga_struct)
	{
		free(tga_struct->img_buf);
		free(tga_struct->bmap);
		free(tga_struct);
		info->_priv_ptr = NULL;
	}
}
