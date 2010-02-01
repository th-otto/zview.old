/* exif-data.c
 *
 * Copyright © 2001 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "exif-mnote-data.h"
#include "exif-data.h"
#include "exif-ifd.h"
#include "exif-mnote-data-priv.h"
#include "exif-utils.h"
#include "exif-loader.h"

#include "jpeg-marker.h"

#include "olympus/exif-mnote-data-olympus.h"
#include "canon/exif-mnote-data-canon.h"
#include "pentax/exif-mnote-data-pentax.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

struct _ExifDataPrivate
{
	ExifByteOrder order;

	ExifMnoteData *md;

	ExifMem *mem;

	unsigned int ref_count;

	/* Temporarily used while loading data */
	unsigned int offset_mnote;
};

static void *
exif_data_alloc (ExifData *data, unsigned int i)
{
	void *d;

	if (!data || !i) return NULL;

	d = exif_mem_alloc (data->priv->mem, i);
	if (d) return d;

	return NULL;
}

ExifMnoteData *
exif_data_get_mnote_data (ExifData *d)
{
	return (d && d->priv) ? d->priv->md : NULL;
}

ExifData *
exif_data_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifData *d = exif_data_new_mem (mem);

	exif_mem_unref (mem);

	return d;
}

ExifData *
exif_data_new_mem (ExifMem *mem)
{
	ExifData *data;
	unsigned int i;

	if (!mem) return NULL;

	data = exif_mem_alloc (mem, sizeof (ExifData));
	if (!data) return (NULL);
	data->priv = exif_mem_alloc (mem, sizeof (ExifDataPrivate));
	if (!data->priv) { exif_mem_free (mem, data); return (NULL); }
	data->priv->ref_count = 1;

	data->priv->mem = mem;
	exif_mem_ref (mem);

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		data->ifd[i] = exif_content_new_mem (data->priv->mem);
		if (!data->ifd[i]) {
			exif_data_free (data);
			return (NULL);
		}
		data->ifd[i]->parent = data;
	}

	return (data);
}

ExifData *
exif_data_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifData *edata;

	edata = exif_data_new ();
	exif_data_load_data (edata, data, size);
	return (edata);
}

static void
exif_data_load_data_entry (ExifData *data, ExifEntry *entry,
			   const unsigned char *d,
			   unsigned int size, unsigned int offset)
{
	unsigned int s, doff;

	entry->tag        = exif_get_short (d + offset + 0, data->priv->order);
	entry->format     = exif_get_short (d + offset + 2, data->priv->order);
	entry->components = exif_get_long  (d + offset + 4, data->priv->order);


	/*
	 * Size? If bigger than 4 bytes, the actual data is not
	 * in the entry but somewhere else (offset).
	 */
	s = exif_format_get_size (entry->format) * entry->components;
	if (!s)
		return;
	if (s > 4)
		doff = exif_get_long (d + offset + 8, data->priv->order);
	else
		doff = offset + 8;

	/* Sanity check */
	if (size < doff + s)
		return;

	entry->data = exif_data_alloc (data, s);
	if (entry->data) {
		entry->size = s;
		memcpy (entry->data, d + doff, s);
	}

	/* If this is the MakerNote, remember the offset */
	if (entry->tag == EXIF_TAG_MAKER_NOTE) {
		data->priv->offset_mnote = doff;
	}

	exif_entry_fix (entry);
}



static void
exif_data_load_data_thumbnail (ExifData *data, const unsigned char *d,
			       unsigned int ds, ExifLong offset, ExifLong size)
{
	if (ds < offset + size) {
		return;
	}
	if (data->data) exif_mem_free (data->priv->mem, data->data);
	data->size = size;
	data->data = exif_data_alloc (data, data->size);
	if (!data->data) return;
	memcpy (data->data, d + offset, data->size);
}

#undef CHECK_REC
#define CHECK_REC(i) 					\
if (data->ifd[(i)] == ifd) {				\
	break;						\
}							\
if (data->ifd[(i)]->count) {				\
	break;						\
}

static void
exif_data_load_data_content (ExifData *data, ExifContent *ifd,
			     const unsigned char *d,
			     unsigned int ds, unsigned int offset)
{
	ExifLong o, thumbnail_offset = 0, thumbnail_length = 0;
	ExifShort n;
	ExifEntry *entry;
	unsigned int i;
	ExifTag tag;

	if (!data || !data->priv) return;

	/* Read the number of entries */
	if (offset >= ds - 1) return;
	n = exif_get_short (d + offset, data->priv->order);

	offset += 2;

	/* Check if we have enough data. */
	if (offset + 12 * n > ds) n = (ds - offset) / 12;

	for (i = 0; i < n; i++) {

		tag = exif_get_short (d + offset + 12 * i, data->priv->order);
		switch (tag) {
		case EXIF_TAG_EXIF_IFD_POINTER:
		case EXIF_TAG_GPS_INFO_IFD_POINTER:
		case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
			o = exif_get_long (d + offset + 12 * i + 8,
					   data->priv->order);
			switch (tag) {
			case EXIF_TAG_EXIF_IFD_POINTER:
				CHECK_REC (EXIF_IFD_EXIF);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_EXIF], d, ds, o);
				break;
			case EXIF_TAG_GPS_INFO_IFD_POINTER:
				CHECK_REC (EXIF_IFD_GPS);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_GPS], d, ds, o);
				break;
			case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
				CHECK_REC (EXIF_IFD_INTEROPERABILITY);
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_INTEROPERABILITY], d, ds, o);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
				thumbnail_offset = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
				thumbnail_length = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			default:
				return;
			}
			break;
		default:

			/*
			 * If we don't know the tag, chances are high
			 * that the EXIF data does not follow the standard.
			 */
			if (!exif_tag_get_name (tag)) {
				return;
			}
			entry = exif_entry_new_mem (data->priv->mem);
			exif_data_load_data_entry (data, entry, d, ds,
						   offset + 12 * i);
			exif_content_add_entry (ifd, entry);
			exif_entry_unref (entry);
			break;
		}
	}
}

typedef enum {
	EXIF_DATA_TYPE_MAKER_NOTE_NONE		= 0,
	EXIF_DATA_TYPE_MAKER_NOTE_CANON		= 1,
	EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS	= 2,
	EXIF_DATA_TYPE_MAKER_NOTE_PENTAX	= 3
} ExifDataTypeMakerNote;

static ExifDataTypeMakerNote
exif_data_get_type_maker_note (ExifData *d)
{
	ExifEntry *e, *em;
	char value[1024];

	if (!d) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;
	
	e = exif_data_get_entry (d, EXIF_TAG_MAKER_NOTE);
        if (!e) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;

	/* Olympus & Nikon */
	if ((e->size >= 5) && (!memcmp (e->data, "OLYMP", 5) ||
			       !memcmp (e->data, "Nikon", 5)))
		return EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS;

	em = exif_data_get_entry (d, EXIF_TAG_MAKE);
	if (!em) return EXIF_DATA_TYPE_MAKER_NOTE_NONE;

	/* Canon */
	if (!strcmp (exif_entry_get_value (em, value, sizeof(value)), "Canon"))
		return EXIF_DATA_TYPE_MAKER_NOTE_CANON;

	/* Pentax & some variant of Nikon */
	if ((e->size >= 2) && (e->data[0] == 0x00) && (e->data[1] == 0x1b)) {
		if (!strncasecmp (
			exif_entry_get_value (em, value, sizeof(value)),
					      "Nikon", 5))
			return EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS;
		else
			return EXIF_DATA_TYPE_MAKER_NOTE_PENTAX;
	}

	return EXIF_DATA_TYPE_MAKER_NOTE_NONE;
}

void
exif_data_load_data (ExifData *data, const unsigned char *d_orig,
		     unsigned int ds_orig)
{
	unsigned int l;
	ExifLong offset;
	ExifShort n;
	const unsigned char *d = d_orig;
	unsigned int ds = ds_orig, len;

	if (!data || !data->priv || !d || !ds) return;


	/*
	 * It can be that the data starts with the EXIF header. If it does
	 * not, search the EXIF marker.
	 */
	if (ds < 6) {
		return;
	}
	if ( memcmp (d, ExifHeader, 6)){
		while (1) {
			while ((d[0] == 0xff) && ds) {
				d++;
				ds--;
			}

			/* JPEG_MARKER_SOI */
			if (d[0] == JPEG_MARKER_SOI) {
				d++;
				ds--;
				continue;
			}

			/* JPEG_MARKER_APP0 */
			if (d[0] == JPEG_MARKER_APP0) {
				d++;
				ds--;
				l = (d[0] << 8) | d[1];
				if (l > ds)
					return;
				d += l;
				ds -= l;
				continue;
			}

			/* JPEG_MARKER_APP1 */
			if (d[0] == JPEG_MARKER_APP1)
				break;

			/* Unknown marker or data. Give up. */

			return;
		}
		d++;
		ds--;
		if (ds < 2) {

			return;
		}
		len = (d[0] << 8) | d[1];

		d += 2;
		ds -= 2;
	}

	/*
	 * Verify the exif header
	 * (offset 2, length 6).
	 */
	if (ds < 6) {

		return;
	}
	if (memcmp (d, ExifHeader, 6)) {

		return;
	}


	/* Byte order (offset 6, length 2) */
	if (ds < 14)
		return;
	if (!memcmp (d + 6, "II", 2))
		data->priv->order = EXIF_BYTE_ORDER_INTEL;
	else if (!memcmp (d + 6, "MM", 2))
		data->priv->order = EXIF_BYTE_ORDER_MOTOROLA;
	else
		return;

	/* Fixed value */
	if (exif_get_short (d + 8, data->priv->order) != 0x002a)
		return;

	/* IFD 0 offset */
	offset = exif_get_long (d + 10, data->priv->order);


	/* Parse the actual exif data (usually offset 14 from start) */
	exif_data_load_data_content (data, data->ifd[EXIF_IFD_0], d + 6,
				     ds - 6, offset);

	/* IFD 1 offset */
	if (offset + 6 + 2 > ds) {
		return;
	}
	n = exif_get_short (d + 6 + offset, data->priv->order);
	if (offset + 6 + 2 + 12 * n + 4 > ds) {
		return;
	}
	offset = exif_get_long (d + 6 + offset + 2 + 12 * n, data->priv->order);
	if (offset) {


		/* Sanity check. */
		if (offset > ds - 6) {

			return;
		}

		exif_data_load_data_content (data, data->ifd[EXIF_IFD_1], d + 6,
					     ds - 6, offset);
	}

	/*
	 * If we got an EXIF_TAG_MAKER_NOTE, try to interpret it. Some
	 * cameras use pointers in the maker note tag that point to the
	 * space between IFDs. Here is the only place where we have access
	 * to that data.
	 */
	switch (exif_data_get_type_maker_note (data)) {
	case EXIF_DATA_TYPE_MAKER_NOTE_OLYMPUS:
		data->priv->md = exif_mnote_data_olympus_new (data->priv->mem);
		break;
	case EXIF_DATA_TYPE_MAKER_NOTE_PENTAX:
		data->priv->md = exif_mnote_data_pentax_new (data->priv->mem);
		break;
	case EXIF_DATA_TYPE_MAKER_NOTE_CANON:
		data->priv->md = exif_mnote_data_canon_new (data->priv->mem);
		break;
	default:
		break;
	}

	/* 
	 * If we are able to interpret the maker note, do so.
	 */
	if (data->priv->md) {
		exif_mnote_data_set_byte_order (data->priv->md,
						data->priv->order);
		exif_mnote_data_set_offset (data->priv->md,
					    data->priv->offset_mnote);
		exif_mnote_data_load (data->priv->md, d, ds);
	}
}


void
exif_data_ref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count++;
}

void
exif_data_unref (ExifData *data)
{
	if (!data) return;

	data->priv->ref_count--;
	if (!data->priv->ref_count) exif_data_free (data);
}

void
exif_data_free (ExifData *data)
{
	unsigned int i;
	ExifMem *mem = (data && data->priv) ? data->priv->mem : NULL;

	if (!data) return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i]) {
			exif_content_unref (data->ifd[i]);
			data->ifd[i] = NULL;
		}
	}

	if (data->data) {
		exif_mem_free (mem, data->data);
		data->data = NULL;
	}

	if (data->priv) {
		if (data->priv->md) {
			exif_mnote_data_unref (data->priv->md);
			data->priv->md = NULL;
		}
		exif_mem_free (mem, data->priv);
		exif_mem_free (mem, data);
	}

	exif_mem_unref (mem);
}


ExifByteOrder
exif_data_get_byte_order (ExifData *data)
{
	if (!data)
		return (0);

	return (data->priv->order);
}

void
exif_data_foreach_content (ExifData *data, ExifDataForeachContentFunc func,
			   void *user_data)
{
	unsigned int i;

	if (!data || !func)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		func (data->ifd[i], user_data);
}

typedef struct _ByteOrderChangeData ByteOrderChangeData;
struct _ByteOrderChangeData {
	ExifByteOrder old, new;
};
