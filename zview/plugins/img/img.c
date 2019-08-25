/*
   notes: verified hyper paint with XPORTIMG.PRG VERSION 5.0 -> output iff
   ------------------------------------------------------------------------------
   1.00 1st release, added hyper paint support
        added 15-bit and 16-bit high-color support
   1.01 updated hyper paint support
   1.02 fixed 16-bit high-color decoding, added 32-bit true-color support
   1.03 removed printf() debug calls
   1.04 fixed hyper paint mono reveresed images
   1.05 minor changes to iff style decoding
   1.06 more changes to iff style decoding, fixed grayscale palettes
   1.07 added ventura publisher support
   1.08 added EGA 16 color palette, if palette is missing
   1.09 made arrays global, better error handling
   1.10 reworked debug info, firebee fix (patched purec library)
   1.11 fixed missing 256 color palette, added ventura 24-bit support
        fixed nosig 256 color ordering
   1.12 Code cleanup, conversion to SLB
 */

#define	VERSION	0x0112
#define AUTHOR "Lonny Pursell"

#include "plugin.h"
#include "zvplugin.h"

#define XIMG  0x58494D47

typedef struct
{
	int16_t		version;
	int16_t		length;
	int16_t		planes;
	int16_t		pattern_length;
	int16_t		pix_width;
	int16_t		pix_height;
	int16_t		width;
	int16_t		height;
} IMGHDR;

typedef struct _picture
{
	int16_t hc;
	uint32_t bpp;
	uint32_t bps;
	uint8_t *bmap;
} IMG_REF;

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("IMG\0");
	}
	return -ENOSYS;
}
#endif

#define DEBUG 0

#include "pal.h"
#include "lof.c"
#include "unpack.c"

boolean __CDECL reader_init(const char *name, IMGINFO info)
{
	int16_t eh, cf, xb, i, j;
	int16_t vdi, xbios, skip, flip, vent;
	uint32_t fl, cd, bms, bitps;
	uint16_t bitpp;
	int32_t r, g, b;
	uint8_t *temp;
	IMGHDR hdr;
	int16_t handle;
	uint8_t head[(256 * 6) + 256];
	IMG_REF *ref;
	
#if DEBUG
	int pflg = (Kbshift(-1) & K_ALT) != 0;				/* alternate key */

	if (pflg)
		printf("\n%s\n", name);
#endif

	temp = NULL;

	if ((handle = (int16_t) Fopen(name, FO_READ)) < 0)
	{
		return FALSE;
	}
	fl = lof(handle);
	if (Fread(handle, sizeof(hdr), &hdr) != sizeof(hdr))
	{
		Fclose(handle);
		return FALSE;
	}
	
#if DEBUG
	if (pflg)
	{
		printf("vn=%i\n", hdr.version);
		printf("hl=%i\n", hdr.length);
		printf("planes=%i\n", hdr.planes);
		printf("pattern len=%i\n", hdr.pattern_length);
		printf("microns w/h=%i/%i\n", hdr.pix_width, hdr.pix_height);
		printf("image w/h=%i/%i\n", hdr.width, hdr.height);
	}
#endif

	ref = (IMG_REF *)malloc(sizeof(*ref));
	if (ref == NULL)
	{
		Fclose(handle);
		return FALSE;
	}

	if (hdr.planes == 15)
	{
		hdr.planes++;
		ref->hc = 1;							/* signal 15-bit promoted to 16-bit */
	} else
	{
		ref->hc = 0;
	}

	eh = (hdr.length * 2) - (8 * 2);		/* extended header len */
	cd = fl - ((uint32_t) hdr.length * 2L);	/* cd len */
#if DEBUG
	if (pflg)
	{
		printf("eh=%i\n", eh);
		printf("fl=%lu\n", (unsigned long)fl);
		printf("cd=%lu\n", (unsigned long)cd);
	}
#endif

	if (hdr.length > 8)
	{
		Fread(handle, (uint32_t) eh, head);
	}
	temp = malloc(cd + 256L);
	if (temp == NULL)
	{
		Fclose(handle);
		return FALSE;
	}
	Fread(handle, cd, temp);
	Fclose(handle);

	/* hack - ventura publisher 24-bit */
	cf = 0;
	if (hdr.length == 9)
	{
		int16_t c0 = ((uint16_t) head[0] << 8) | (uint16_t) head[1];

		if (c0 == 3)
		{
			if (hdr.pattern_length == 3)
			{
				hdr.planes = 24;
				cf = 1;
			}
		}
	}
	xb = (hdr.width + 7) / 8;
	bms = ((uint32_t) xb * (uint32_t) hdr.planes) * (uint32_t) hdr.height;
	ref->bps = bms / (uint32_t) hdr.height;		/* bytes per scan line */
	bitps = ref->bps * 8;						/* bits per scan line */
	bitpp = (int16_t) bitps / hdr.planes;	/* bits per plane */
	ref->bpp = bitpp / 8;

#if DEBUG
	if (pflg)
	{
		printf("bms=%lu\n", (unsigned long)bms);
		printf("bps=%lu\n", (unsigned long)ref->bps);
		printf("bpp=%lu\n", (unsigned long)ref->bpp);
	}
#endif

	ref->bmap = malloc(bms + 256L);
	if (ref->bmap == NULL)
	{
		free(ref);
		free(temp);
		return FALSE;
	}

	/* decomp */
	if (cf == 0)
	{
		if (decode_standard(temp, ref->bmap, hdr.height, hdr.pattern_length, ref->bps) != 0)
		{
			free(ref->bmap);
			free(ref);
			free(temp);
			return FALSE;
		}
	} else if (cf == 1)
	{
		if (decode_ventura(temp, ref->bmap, bms) != bms)
		{
			free(ref->bmap);
			free(ref);
			free(temp);
			return FALSE;
		}
	}

	free(temp);

	switch (hdr.planes)
	{
	case 1:
	case 16:
	case 24:
	case 32:
		info->indexed_color = FALSE;
		break;
	case 2:
	case 4:
	case 8:
		info->indexed_color = TRUE;
		break;
	default:
		free(ref->bmap);
		free(ref);
		return FALSE;
	}

	info->planes = hdr.planes;
	info->width = hdr.width;
	info->height = hdr.height;
	info->components = info->planes == 1 ? 1 : 3;
	if (hdr.planes == 32)
	{
		info->colors = 1L << 24;
	} else
	{
		info->colors = 1L << info->planes;
	}
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->page = 1;
	info->orientation = UP_TO_DOWN;
	info->num_comments = 0;

	strcpy(info->info, "GEM Bitmap");
	strcpy(info->compression, "RLE");

	/* detemine palette type */
	vdi = xbios = skip = flip = vent = 0;
	if (hdr.length == 8)
	{
		if (hdr.planes == 1)
		{
			strcat(info->info, " (Standard)");
		} else
		{
			strcat(info->info, " (Ventura Publisher)");
#if DEBUG
			if (pflg)
				printf("ventura8\n");
#endif
			vent = 1;
		}
	} else if (hdr.length == 9)
	{
		strcat(info->info, " (Ventura Publisher)");
		skip = 2;
		vent = 1;
	} else
	{
		if (head[0] == 'X' && head[1] == 'I' && head[2] == 'M' && head[3] == 'G')
		{								/* ximg? */
			int16_t type = ((uint16_t) head[4] << 8) | (uint16_t) head[5];

#if DEBUG
			if (pflg)
				printf("ximg type=%i\n", 0);
#endif
			if (type)
			{							/* profibuch page 1267 -> 0=rgb 1=cmy 2=pantone */
#if DEBUG
				if (pflg)
					printf("XIMG type not rgb=%i\n", type);
#endif
				free(ref->bmap);
				free(ref);
				return FALSE;
			}
			strcat(info->info, " (XIMG)");
			skip = 4 + 2;				/* skip id + pal type flag */
			vdi = 1;
		} else if (head[0] == 'S' && head[1] == 'T' && head[2] == 'T' && head[3] == 'T')
		{								/* sttt? */
#if DEBUG
			if (pflg)
			{
				int16_t count = ((uint16_t) head[4] << 8) | (uint16_t) head[5];

				printf("sttt count=%i\n", count);
			}
#endif
			strcat(info->info, " (STTT)");
			skip = 4 + 2;				/* skip id + color count */
			xbios = 1;
		} else if (head[0] == 'T' && head[1] == 'I' && head[2] == 'M' && head[3] == 'G')
		{								/* timg? */
#if DEBUG
			if (pflg)
				printf("TIMG err\n");
#endif
			free(ref->bmap);
			free(ref);
			return FALSE;
		} else if (head[0] == 0x00 && head[1] == 0x80)
		{								/* hyperpaint? */
			int16_t c0 = ((uint16_t) head[2] << 8) | (uint16_t) head[3];
			int16_t c1 = ((uint16_t) head[4] << 8) | (uint16_t) head[5];

			if (c0 == c1)
			{
				flip = 1;
			}
			strcat(info->info, " (Hyper Paint)");
			skip = 2;					/* skip id */
			xbios = 1;
		} else
		{
			strcat(info->info, " (Unknown)");
#if DEBUG
			if (pflg)
				printf("unknown header type\n");
#endif
			if (eh == 4 * 2 || eh == 16 * 2 || eh == 256 * 2)
			{							/* assume xbios palette */
				skip = 0;
				xbios = 1;
			} else if (eh == 4 * 6 || eh == 16 * 6 || eh == 256 * 6)
			{
				skip = 0;
				vdi = 1;
			}
		}
	}

#if DEBUG
	if (pflg)
		printf("vdi=%i xbios=%i vent=%i\n", vdi, xbios, vent);
#endif

	/* process palette */
	if (info->indexed_color)
	{
		if (vdi)
		{								/* vdi format in hardware order */
			j = skip;
			for (i = 0; i < info->colors; i++)
			{
				r = ((uint16_t) head[j] << 8) | (uint16_t) head[j + 1];
				g = ((uint16_t) head[j + 2] << 8) | (uint16_t) head[j + 3];
				b = ((uint16_t) head[j + 4] << 8) | (uint16_t) head[j + 5];
				j = j + 6;
				info->palette[i].red = (uint8_t) ((((r << 8) - r) + 500) / 1000L);
				info->palette[i].green = (uint8_t) ((((g << 8) - g) + 500) / 1000L);
				info->palette[i].blue = (uint8_t) ((((b << 8) - b) + 500) / 1000L);
			}
		} else if (xbios)
		{								/* xbios format */
			j = skip;
			for (i = 0; i < info->colors; i++)
			{
				uint16_t rgb = ((uint16_t) head[j] << 8) | (uint16_t) head[j + 1];

				j = j + 2;
				info->palette[i].red = (((rgb >> 7) & 0xE) + ((rgb >> 11) & 0x1)) * 17;
				info->palette[i].green = (((rgb >> 3) & 0xE) + ((rgb >> 7) & 0x1)) * 17;
				info->palette[i].blue = (((rgb << 1) & 0xE) + ((rgb >> 3) & 0x1)) * 17;
			}
		} else if (vent)
		{								/* ventura? (no palette) */
			int16_t gflag = 0;

			if (hdr.length == 9)
			{
				gflag = ((uint16_t) head[0] << 8) | (uint16_t) head[1];
			}
#if DEBUG
			if (pflg)
				printf("gflag=%i\n", gflag);
#endif
			if (gflag == 0)
			{							/* color? */
				if (info->colors == 4)
				{						/* force gray */
					for (i = 0; i < 4; i++)
					{
						info->palette[i].red = info->palette[i].green = info->palette[i].blue = vent_g4[i];
					}
				} else if (info->colors == 16)
				{
					j = 0;
					for (i = 0; i < 16; i++)
					{
						info->palette[i].red = vent_c16[j++] << 2;
						info->palette[i].green = vent_c16[j++] << 2;
						info->palette[i].blue = vent_c16[j++] << 2;
					}
				} else if (info->colors == 256)
				{						/* force gray */
					for (i = 0; i < 256; i++)
					{
						info->palette[i].red = info->palette[i].green = info->palette[i].blue = vent_g256[i];
					}
				}
			} else if (gflag == 1)
			{							/* grayscale? */
				if (info->colors == 4)
				{						/* don't know if 4 color ones exist */
					for (i = 0; i < 4; i++)
					{
						info->palette[i].red = info->palette[i].green = info->palette[i].blue = vent_g4[i];
					}
				} else if (info->colors == 16)
				{
					for (i = 0; i < 16; i++)
					{
						info->palette[i].red = info->palette[i].green = info->palette[i].blue = vent_g16[i];
					}
				} else if (info->colors == 256)
				{
					for (i = 0; i < 256; i++)
					{
						info->palette[i].red = info->palette[i].green = info->palette[i].blue = vent_g256[i];
					}
				}
			}
		}
	}

	if (info->planes == 1)
	{									/* mono */
		if (flip)
		{								/* has palette */
			uint8_t *bmap = ref->bmap;

#if DEBUG
			if (pflg)
				printf("mono reversed\n");
#endif
			for (i = 0; i < bms; i++)
			{
				bmap[i] = ~bmap[i];
			}
		}
	}

	info->_priv_ptr = ref;
	info->_priv_var = 0;				/* y pos */

	return TRUE;
}


boolean __CDECL reader_read(IMGINFO info, uint8_t *buffer)
{
	IMG_REF *ref = (IMG_REF *)info->_priv_ptr;
	uint8_t *bmap = ref->bmap;
	uint32_t pos = info->_priv_var;

	switch (info->planes)
	{
	case 1:
		{
			int16_t bit;
			int16_t p0;
			int16_t x;

			x = (int16_t)ref->bps;
			info->_priv_var += x;
			x >>= 3;
			bmap += pos;
			do
			{							/* 1-bit mono v1.00 */
				p0 = *bmap++;
				for (bit = 7; bit >= 0; bit--)
				{
					*buffer = (p0 >> bit) & 1;
				}
			} while (--x > 0);
		}
		break;
	case 2:
	case 4:
	case 8:
		{								/* basically the same layout as iff */
			uint16_t x, i;

			info->_priv_var += ref->bps;
			for (x = 0; x < info->width; x++)
			{
				uint16_t c = 0;

				for (i = 0; i < info->planes; i++)
				{
					if (bmap[pos + (i * ref->bpp) + (x >> 3)] & (1 << (7 - (x & 7))))
					{					/* is set? */
						c = c | (1 << i);
					}
				}
				buffer[x] = c;
			}
		}
		break;
	case 16:
		{
			uint16_t x;
			uint16_t i = 0;

			info->_priv_var += ref->bps;
			if (ref->hc)
			{							/* xrrrrrgggggbbbbb */
				for (x = 0; x < info->width; x++)
				{
					uint16_t rgb = *(uint16_t *) &bmap[pos];

					pos = pos + 2;
					buffer[i++] = ((rgb >> 10) & 0x001F) << 3;
					buffer[i++] = ((rgb >> 5) & 0x001F) << 3;
					buffer[i++] = (rgb & 0x001F) << 3;
				}
			} else
			{							/* rrrrrggggggbbbbb */
				for (x = 0; x < info->width; x++)
				{
					uint16_t rgb = *(uint16_t *) &bmap[pos];

					pos = pos + 2;
					buffer[i++] = ((rgb >> 11) & 0x001F) << 3;
					buffer[i++] = ((rgb >> 5) & 0x003F) << 2;
					buffer[i++] = (rgb & 0x001F) << 3;
				}
			}
		}
		break;
	case 24:							/* rrrrrrrrggggggggbbbbbbbb */
		{
			memcpy(buffer, &bmap[info->_priv_var], (uint32_t) info->width * 3L);
			info->_priv_var += ref->bps;
		}
		break;
	case 32:
		{								/* xxxxxxxxrrrrrrrrggggggggbbbbbbbb */
			int16_t i = 0, x;

			info->_priv_var += ref->bps;
			for (x = 0; x < info->width; x++)
			{
				pos++;					/* skip */
				buffer[i++] = bmap[pos++];
				buffer[i++] = bmap[pos++];
				buffer[i++] = bmap[pos++];
			}
		}
		break;
	}

	return TRUE;
}


void __CDECL reader_get_txt(IMGINFO info, txt_data *txtdata)
{
	(void)info;
	(void)txtdata;
}


void __CDECL reader_quit(IMGINFO info)
{
	IMG_REF *ref = (IMG_REF *)info->_priv_ptr;
	
	if (ref)
	{
		free(ref->bmap);
		free(ref);
		info->_priv_ptr = NULL;
	}
}
