//========================================================================
//
// BuiltinFont.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef BUILTINFONT_H
#define BUILTINFONT_H

#include <aconf.h>

#include "gtypes.h"

struct BuiltinFont;

//------------------------------------------------------------------------

struct BuiltinFontWidth {
  const char *name;
  Gushort width;
  const struct BuiltinFontWidth *next;
};

struct BuiltinFont {
  const char *name;
  const char *const *defaultBaseEnc;
  short missingWidth;
  short ascent;
  short descent;
  short bbox[4];
  unsigned int size;
  const struct BuiltinFontWidth *widths;
  const struct BuiltinFontWidth *const *hashtab;
};

GBool BuiltinFontWidths_getWidth(const struct BuiltinFont *font, const char *name, Gushort *width);

#endif
