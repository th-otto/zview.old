//========================================================================
//
// SplashFTFontFile.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHFTFONTFILE_H
#define SPLASHFTFONTFILE_H

#include <aconf.h>

#ifdef HAVE_FREETYPE

#include <ft2build.h>
#include <freetype/freetype.h>
#include "SplashFontFile.h"

class SplashFontFileID;
class SplashFTFontEngine;

//------------------------------------------------------------------------
// SplashFTFontFile
//------------------------------------------------------------------------

class SplashFTFontFile: public SplashFontFile {
public:

  static SplashFontFile *loadType1Font(SplashFTFontEngine *engineA,
				       SplashFontFileID *idA,
				       SplashFontType fontTypeA,
#ifdef LOAD_FONTS_FROM_MEM
				       GString *fontBufA,
#else
				       char *fileNameA, GBool deleteFileA,
#endif
				       const char *const *encA);
  static SplashFontFile *loadCIDFont(SplashFTFontEngine *engineA,
				     SplashFontFileID *idA,
				     SplashFontType fontTypeA,
#ifdef LOAD_FONTS_FROM_MEM
				     GString *fontBufA,
#else
				     char *fileNameA, GBool deleteFileA,
#endif
				     int *codeToGIDA, int codeToGIDLenA);
  static SplashFontFile *loadTrueTypeFont(SplashFTFontEngine *engineA,
					  SplashFontFileID *idA,
					  SplashFontType fontTypeA,
#ifdef LOAD_FONTS_FROM_MEM
					  GString *fontBufA,
#else
					  char *fileNameA,
					  GBool deleteFileA,
#endif
					  int fontNum,
					  int *codeToGIDA,
					  int codeToGIDLenA);

  virtual ~SplashFTFontFile();

  // Create a new SplashFTFont, i.e., a scaled instance of this font
  // file.
  virtual SplashFont *makeFont(SplashCoord *mat,
			       SplashCoord *textMat);

private:

  SplashFTFontFile(SplashFTFontEngine *engineA,
		   SplashFontFileID *idA,
		   SplashFontType fontTypeA,
#ifdef LOAD_FONTS_FROM_MEM
		   GString *fontBufA,
#else
		   char *fileNameA, GBool deleteFileA,
#endif
		   FT_Face faceA,
		   int *codeToGIDA, int codeToGIDLenA);

  SplashFTFontEngine *engine;
  FT_Face face;
  int *codeToGID;
  int codeToGIDLen;

  friend class SplashFTFont;
};

#endif /* HAVE_FREETYPE */

#endif
