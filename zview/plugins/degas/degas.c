/*
  1.00 1st release, added build info, fixed compression info message
  1.01 fixed 1-bit compressed bug, fixed reversed 1-bit images
  1.02 fixed missing fclose()
  1.03 added medium rez scaling
  1.04 firebee fixes (patched purec library)
  1.05 reworked iff to st format code
  1.06 missing fclose() if wrong rez info
  1.07 added overscan support for st low - 320x240, 320x280, 416x560
  1.08 made arrays global, reworked error handling
  1.09 source clean-up
  1.10 added salert support
  1.11 fixed typo in the info section
  1.12 smaller startup module
  1.13 Convert to new style plugin
*/

#include "plugin.h"
#include "zvplugin.h"

#define VERSION		0x0113
#define AUTHOR      "Lonny Pursell"

#define DEGAS_SIZE	32034L
#define ELITE_SIZE	32066L
#define	M_320x240	38434L				/* overscan modes */
#define	M_320x280	44834L
#define	M_416x560	116514L

static uint16_t const iw[4] = { 320, 640, 640, 0 };
static uint16_t const ih[4] = { 200, 200, 400, 0 };
static int16_t const indexed_color[4] = { TRUE, TRUE, FALSE, FALSE };
static uint16_t const components[4] = { 3, 3, 1, 0 };


#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("PI1\0" "PI2\0" "PI3\0" "PC1\0" "PC2\0" "PC3\0");
	}
	return -ENOSYS;
}
#endif

typedef struct
{										/* 34 bytes */
	uint16_t flags;
	uint16_t pal[16];						/* xbios format, xbios order */
} HEADER;


/* to be fixed: */
#define bail(msg)

#include "lof.c"
#include "packbits.c"
#include "tanglbts.c"


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
boolean __CDECL reader_init( const char *name, IMGINFO info)
{
	int16_t handle;
	uint8_t *bmap;
	uint8_t *temp;
	int16_t cf, rez, flip, os;
	uint32_t bms, file_size, bw;
	HEADER hdr;

	if ((handle = (int16_t) Fopen(name, FO_READ)) < 0)
	{
		bail("Invalid path");
		return FALSE;
	}
	file_size = lof(handle);

	if (Fread(handle, sizeof(HEADER), &hdr) != sizeof(HEADER))
	{
		Fclose(handle);
		bail("Fread failed");
	}

	/* flags -> cxxxx?rr */
	/* cf = compression flag */
	/* rr = res 0=low 1=med 2=high */
	/* ?  = set in overscan file 320x240, unreliable flag */

	os = 0;
	cf = hdr.flags & 0x8000;
	rez = hdr.flags & 3;
	info->planes = 4 >> rez;
	if (info->planes == 0)
	{
		/* resolution 3: invalid */
		Fclose(handle);
		bail("unsupported");
		return FALSE;
	}
	flip = hdr.pal[0] & 1;

	info->width = iw[rez];				/* defaults */
	info->height = ih[rez];

	if (file_size <= ELITE_SIZE)
	{									/* assume compressed */
		/* nop */
	} else
	{
		if (info->planes == 4)
		{
			if (file_size == M_320x240)
			{							/* check for overscan */
				info->height = 240;
				os = 1;
			} else if (file_size == M_320x280)
			{
				info->height = 280;
				os = 1;
			} else if (file_size == M_416x560)
			{
				info->width = 416;
				info->height = 560;
				os = 1;
			} else
			{
				Fclose(handle);
				bail("File size wrong");
				return FALSE;
			}
		} else
		{
			Fclose(handle);
			bail("File size wrong");
			return FALSE;
		}
	}

	bw = ((((uint32_t) info->width + 15L) / 16L) * 2L) * (uint32_t) info->planes;
	bms = bw * (uint32_t) info->height;
	bmap = malloc(bms);
	if (bmap == NULL)
	{
		Fclose(handle);
		bail("Malloc(bitmap) failed");
		return FALSE;
	}

	if (cf)
	{
		if (Fread(handle, file_size - sizeof(HEADER), bmap) != file_size - sizeof(HEADER))
		{
			free(bmap);
			Fclose(handle);
			bail("Fread failed");
			return FALSE;
		}
		temp = malloc(bms + 256L);
		if (temp == NULL)
		{
			free(bmap);
			Fclose(handle);
			bail("Malloc(decompression) failed");
			return FALSE;
		}
		decode_packbits(temp, bmap, bms);
		tangle_bitplanes(bmap, temp, info->width, info->height, info->planes);
		free(temp);
		strcpy(info->compression, "RLE");
	} else
	{
		Fread(handle, bms, bmap);
		strcpy(info->compression, "None");
	}

	Fclose(handle);

	switch (info->planes)
	{
	case 2:							/* medium? - start scale */
		if (Kbshift(-1) & 0x10)
		{								/* caps lock? */
			int16_t i;

			temp = malloc(bms);
			if (temp == NULL)
			{
				free(bmap);
				bail("Malloc(scale) failed");
				return FALSE;
			}
			memcpy(temp, bmap, bms);
			free(bmap);
			bmap = malloc(bms * 2L);
			if (bmap == NULL)
			{
				free(temp);
				bail("Malloc(rescale) failed");
				return FALSE;
			}
			for (i = 0; i < info->height; i++)
			{
				memcpy(bmap + ((i << 1L) * bw), temp + (i * bw), bw);
				memcpy(bmap + (((i << 1L) + 1L) * bw), temp + (i * bw), bw);
			}
			free(temp);
			info->height = info->height * 2;	/* adjust */
		}
		break;
	case 1:
		if (!flip)
		{
			uint32_t i;

			for (i = 0; i < bms; i++)
			{
				bmap[i] = ~bmap[i];
			}
		}
		break;
	}

	info->components = components[rez];
	info->indexed_color = indexed_color[rez];
	info->colors = 1L << (uint32_t) info->planes;
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->orientation = UP_TO_DOWN;
	info->page = 1;						/* required - more than 1 = animation */
	info->num_comments = 0;				/* required - disable exif tab */
	info->_priv_var = 0;				/* y position in bmap */
	info->_priv_ptr = bmap;

	strcpy(info->info, "Degas");
	if (file_size == ELITE_SIZE)
	{
		strcat(info->info, " Elite");
	}
	if (os)
	{
		strcat(info->info, " (Overscan)");
	}

	if (info->indexed_color)
	{
		int16_t i;

		for (i = 0; i < info->colors; i++)
		{
			uint16_t c;
			c = (((hdr.pal[i] >> 7) & 0xE) + ((hdr.pal[i] >> 11) & 0x1));
			info->palette[i].red = (c << 4) | c;
			c = (((hdr.pal[i] >> 3) & 0xE) + ((hdr.pal[i] >> 7) & 0x1));
			info->palette[i].green = (c << 4) | c;
			c = (((hdr.pal[i] << 1) & 0xE) + ((hdr.pal[i] >> 3) & 0x1));
			info->palette[i].blue = (c << 4) | c;
		}
	}

	return TRUE;
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
	int16_t bit;
	int16_t x;
	int32_t offset;
	
	offset = info->_priv_var;
	if (info->planes == 1)
	{
		int16_t p0;
		uint8_t *bmap = (uint8_t *)info->_priv_ptr + offset;

		x = info->width >> 3;
		info->_priv_var += x;
		do
		{								/* 1-bit mono v1.00 */
			p0 = *bmap++;
			for (bit = 7; bit >= 0; bit--)
			{
				*buffer++ = (p0 >> bit) & 1;
			}
		} while (--x > 0);
	} else
	{
		int16_t ndx;
		int16_t pln;
		uint16_t *xmap = (uint16_t *)info->_priv_ptr + offset;	/* as word array */

		x = info->width >> 4;
		info->_priv_var += x * info->planes;
		do
		{								/* 1-bit to 8-bit atari st word interleaved bitmap v1.01 */
			for (bit = 15; bit >= 0; bit--)
			{
				ndx = 0;
				for (pln = 0; pln < info->planes; pln++)
				{
					if ((xmap[pln] >> bit) & 1)
					{
						ndx |= 1 << pln;
					}
				}
				*buffer++ = ndx;
			}
			xmap += info->planes;	/* next plane */
		} while (--x > 0);		/* next x */
	}

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function must be always present.					*
 *		It fills txtdata struct. with the text present in the picture ( if any).	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		txtdata		->	The destination text buffer.								*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata)
{
	(void)info;
	(void)txtdata;
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
	free(info->_priv_ptr);
	info->_priv_ptr = NULL;
}
