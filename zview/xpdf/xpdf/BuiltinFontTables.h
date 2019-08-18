//========================================================================
//
// BuiltinFontTables.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef BUILTINFONTTABLES_H
#define BUILTINFONTTABLES_H

#include "BuiltinFont.h"

#define nBuiltinFonts      14
#define nBuiltinFontSubsts 12

extern BuiltinFont const builtinFonts[nBuiltinFonts];
extern const BuiltinFont *const builtinFontSubst[nBuiltinFontSubsts];

#if 0
extern void initBuiltinFontTables();
extern void freeBuiltinFontTables();
#endif

#endif
