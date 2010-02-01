/* exif-content.c
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

#include "exif-content.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

struct _ExifContentPrivate
{
	unsigned int ref_count;

	ExifMem *mem;
};

ExifContent *
exif_content_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifContent *content = exif_content_new_mem (mem);

	exif_mem_unref (mem);

	return content;
}

ExifContent *
exif_content_new_mem (ExifMem *mem)
{
	ExifContent *content;

	if (!mem) return NULL;

	content = exif_mem_alloc (mem, (ExifLong) sizeof (ExifContent));
	if (!content)
		return NULL;
	content->priv = exif_mem_alloc (mem,
				(ExifLong) sizeof (ExifContentPrivate));
	if (!content->priv) {
		exif_mem_free (mem, content);
		return NULL;
	}

	content->priv->ref_count = 1;

	content->priv->mem = mem;
	exif_mem_ref (mem);

	return content;
}

void
exif_content_ref (ExifContent *content)
{
	content->priv->ref_count++;
}

void
exif_content_unref (ExifContent *content)
{
	content->priv->ref_count--;
	if (!content->priv->ref_count)
		exif_content_free (content);
}

void
exif_content_free (ExifContent *content)
{
	ExifMem *mem = (content && content->priv) ? content->priv->mem : NULL;
	unsigned int i;

	if (!content) return;

	for (i = 0; i < content->count; i++)
		exif_entry_unref (content->entries[i]);
	exif_mem_free (mem, content->entries);

	exif_mem_free (mem, content->priv);
	exif_mem_free (mem, content);
	exif_mem_unref (mem);
}


void
exif_content_add_entry (ExifContent *c, ExifEntry *entry)
{
	if (!c || !c->priv || !entry || entry->parent) return;

	if (exif_content_get_entry (c, entry->tag)) {
		return;
	}

	entry->parent = c;
	c->entries = exif_mem_realloc (c->priv->mem,
		c->entries, sizeof (ExifEntry) * (c->count + 1));
	if (!c->entries) return;
	c->entries[c->count] = entry;
	exif_entry_ref (entry);
	c->count++;
}
/*
void
exif_content_remove_entry (ExifContent *c, ExifEntry *e)
{
	unsigned int i;

	if (!c || !c->priv || !e || (e->parent != c)) return;

	for (i = 0; i < c->count; i++) if (c->entries[i] == e) break;
	if (i == c->count) return;

	memmove (&c->entries[i], &c->entries[i + 1],
		 sizeof (ExifEntry) * (c->count - i - 1));
	c->count--;
	e->parent = NULL;
	exif_entry_unref (e);
	c->entries = exif_mem_realloc (c->priv->mem, c->entries,
					sizeof(ExifEntry) * c->count);
}
*/
ExifEntry *
exif_content_get_entry (ExifContent *content, ExifTag tag)
{
	unsigned int i;

	if (!content)
		return (NULL);

	for (i = 0; i < content->count; i++)
		if (content->entries[i]->tag == tag)
			return (content->entries[i]);
	return (NULL);
}

void
exif_content_foreach_entry (ExifContent *content,
			    ExifContentForeachEntryFunc func, void *data)
{
	unsigned int i;

	if (!content || !func)
		return;

	for (i = 0; i < content->count; i++)
		func (content->entries[i], data);
}
