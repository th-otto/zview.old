//========================================================================
//
// BuiltinFont.cc
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#include <stdlib.h>
#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "FontEncodingTables.h"
#include "BuiltinFont.h"

//------------------------------------------------------------------------

#if 0
BuiltinFontWidths::BuiltinFontWidths(BuiltinFontWidth *widths, int sizeA) {
  int i, h;

  size = sizeA;
  tab = (BuiltinFontWidth **)gmallocn(size, sizeof(BuiltinFontWidth *));
  for (i = 0; i < size; ++i) {
    tab[i] = NULL;
  }
  for (i = 0; i < sizeA; ++i) {
    h = hash(widths[i].name);
    widths[i].next = tab[h];
    tab[h] = &widths[i];
  }
}
#endif

static unsigned int BuiltinFontWidths_hash(const char *name, unsigned int hashsize) {
  const char *p;
  unsigned int h;

  h = 0;
  for (p = name; *p; ++p) {
    h = 17 * h + (int)(*p & 0xff);
  }
  return h % hashsize;
}

GBool BuiltinFontWidths_getWidth(const struct BuiltinFont *font, const char *name, Gushort *width) {
  unsigned int h;
  const BuiltinFontWidth *p;

  h = BuiltinFontWidths_hash(name, font->size);
  for (p = font->hashtab[h]; p; p = p->next) {
    if (!strcmp(p->name, name)) {
      *width = p->width;
      return gTrue;
    }
  }
  *width = 0;
  return gFalse;
}

