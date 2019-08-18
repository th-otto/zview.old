//========================================================================
//
// FoFiEncodings.h
//
// Copyright 1999-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef FOFIENCODINGS_H
#define FOFIENCODINGS_H

#include <aconf.h>

#include "gtypes.h"

//------------------------------------------------------------------------
// Type 1 and 1C font data
//------------------------------------------------------------------------

extern const char *const fofiType1StandardEncoding[256];
extern const char *const fofiType1ExpertEncoding[256];

//------------------------------------------------------------------------
// Type 1C font data
//------------------------------------------------------------------------

extern const char *const fofiType1CStdStrings[391];
extern Gushort const fofiType1CISOAdobeCharset[229];
extern Gushort const fofiType1CExpertCharset[166];
extern Gushort const fofiType1CExpertSubsetCharset[87];

#endif
