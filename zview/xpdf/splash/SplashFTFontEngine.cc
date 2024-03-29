//========================================================================
//
// SplashFTFontEngine.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef HAVE_FREETYPE

#include <stdio.h>
#ifndef _WIN32
#  include <unistd.h>
#endif
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "gfile.h"
#include "FoFiTrueType.h"
#include "FoFiType1C.h"
#include "SplashFTFontFile.h"
#include "SplashFTFontEngine.h"
#include <freetype/ftmodapi.h>
#ifdef FT_DRIVER_H
#  include <freetype/ftdriver.h>
#else
#  include <freetype/ftcffdrv.h>
#endif
#if defined(ZVPDF_SLB)
#include "../general.h"
#include "../winimg.h"
#include "../pdflib/pdflib.h"
#elif defined(FREETYPE_SLB)
#include <slb/freetype.h>
#endif

#ifdef VMS
#if (__VMS_VER < 70000000)
extern "C" int unlink(char *filename);
#endif
#endif

//------------------------------------------------------------------------

static void fileWrite(void *stream, const char *data, int len) {
  fwrite(data, 1, len, (FILE *)stream);
}

#ifdef LOAD_FONTS_FROM_MEM
static void gstringWrite(void *stream, const char *data, int len) {
  ((GString *)stream)->append(data, len);
}
#endif

//------------------------------------------------------------------------
// SplashFTFontEngine
//------------------------------------------------------------------------

SplashFTFontEngine::SplashFTFontEngine(GBool aaA, Guint flagsA,
				       FT_Library libA) {
  FT_Int major, minor, patch;

  aa = aaA;
  flags = flagsA;
  lib = libA;

  // as of FT 2.1.8, CID fonts are indexed by CID instead of GID
  FT_Library_Version(lib, &major, &minor, &patch);
  useCIDs = major > 2 ||
            (major == 2 && (minor > 1 || (minor == 1 && patch > 7)));
}

SplashFTFontEngine *SplashFTFontEngine::init(GBool aaA, Guint flagsA) {
  FT_Library libA;

#if defined(ZVPDF_SLB)
  if (zvpdf_freetype_open() < 0)
  	 return NULL;
#elif defined(FREETYPE_SLB)
  if (slb_freetype_open(NULL) < 0)
  	 return NULL;
#endif
  if (FT_Init_FreeType(&libA)) {
    return NULL;
  }
  return new SplashFTFontEngine(aaA, flagsA, libA);
}

SplashFTFontEngine::~SplashFTFontEngine() {
  FT_Done_FreeType(lib);
#if defined(ZVPDF_SLB)
  zvpdf_freetype_close();
#elif defined(FREETYPE_SLB)
  slb_freetype_close();
#endif
}

SplashFontFile *SplashFTFontEngine::loadType1Font(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
						  GString *fontBuf,
#else
						  char *fileName,
						  GBool deleteFile,
#endif
						  const char *const *enc) {
  return SplashFTFontFile::loadType1Font(this, idA, splashFontType1,
#ifdef LOAD_FONTS_FROM_MEM
					 fontBuf,
#else
					 fileName, deleteFile,
#endif
					 enc);
}

SplashFontFile *SplashFTFontEngine::loadType1CFont(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
						   GString *fontBuf,
#else
						   char *fileName,
						   GBool deleteFile,
#endif
						   const char **enc) {
  return SplashFTFontFile::loadType1Font(this, idA, splashFontType1C,
#ifdef LOAD_FONTS_FROM_MEM
					 fontBuf,
#else
					 fileName, deleteFile,
#endif
					 enc);
}

SplashFontFile *SplashFTFontEngine::loadOpenTypeT1CFont(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
							GString *fontBuf,
#else
							char *fileName,
							GBool deleteFile,
#endif
							const char **enc) {
  FoFiTrueType *ff;
#ifdef LOAD_FONTS_FROM_MEM
  GString *fontBuf2;
#else
  GString *tmpFileName;
  FILE *tmpFile;
#endif
  SplashFontFile *ret;

#ifdef LOAD_FONTS_FROM_MEM
  if (!(ff = FoFiTrueType::make(fontBuf->getCString(), fontBuf->getLength(),
				0, gTrue))) {
#else
  if (!(ff = FoFiTrueType::load(fileName, 0, gTrue))) {
#endif
    return NULL;
  }
  if (ff->isHeadlessCFF()) {
#ifdef LOAD_FONTS_FROM_MEM
    fontBuf2 = new GString();
    ff->convertToType1(NULL, enc, gFalse, &gstringWrite, fontBuf2);
    delete ff;
    ret = SplashFTFontFile::loadType1Font(this, idA, splashFontType1,
					  fontBuf2, enc);
    if (ret) {
      delete fontBuf;
    } else {
      delete fontBuf2;
    }
#else
    tmpFileName = NULL;
    if (!openTempFile(&tmpFileName, &tmpFile, "wb", NULL)) {
      delete ff;
      return NULL;
    }
    ff->convertToType1(NULL, enc, gFalse, &fileWrite, tmpFile);
    delete ff;
    fclose(tmpFile);
    ret = SplashFTFontFile::loadType1Font(this, idA, splashFontType1,
					  tmpFileName->getCString(),
					  gTrue, enc);
    if (ret) {
      if (deleteFile) {
	unlink(fileName);
      }
    } else {
      unlink(tmpFileName->getCString());
    }
    delete tmpFileName;
#endif
  } else {
    delete ff;
    ret = SplashFTFontFile::loadType1Font(this, idA, splashFontOpenTypeT1C,
#ifdef LOAD_FONTS_FROM_MEM
					  fontBuf,
#else
					  fileName, deleteFile,
#endif
					  enc);
  }
  return ret;
}

SplashFontFile *SplashFTFontEngine::loadCIDFont(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
						GString *fontBuf,
#else
						char *fileName,
						GBool deleteFile,
#endif
						int *codeToGID,
						int codeToGIDLen) {
  FoFiType1C *ff;
  int *cidToGIDMap;
  int nCIDs;
  SplashFontFile *ret;

  // check for a CFF font
  if (codeToGID) {
    cidToGIDMap = NULL;
    nCIDs = 0;
  } else if (useCIDs) {
    cidToGIDMap = NULL;
    nCIDs = 0;
#ifdef LOAD_FONTS_FROM_MEM
  } else if ((ff = FoFiType1C::make(fontBuf->getCString(),
				    fontBuf->getLength()))) {
#else
  } else if ((ff = FoFiType1C::load(fileName))) {
#endif
    cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
    delete ff;
  } else {
    cidToGIDMap = NULL;
    nCIDs = 0;
  }
  ret = SplashFTFontFile::loadCIDFont(this, idA, splashFontCID,
#ifdef LOAD_FONTS_FROM_MEM
				      fontBuf,
#else
				      fileName, deleteFile,
#endif
				      codeToGID ? codeToGID : cidToGIDMap,
				      codeToGID ? codeToGIDLen : nCIDs);
  if (!ret) {
    gfree(cidToGIDMap);
  }
  return ret;
}

SplashFontFile *SplashFTFontEngine::loadOpenTypeCFFFont(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
							GString *fontBuf,
#else
							char *fileName,
							GBool deleteFile,
#endif
							int *codeToGID,
							int codeToGIDLen) {
  FoFiTrueType *ff;
#ifdef LOAD_FONTS_FROM_MEM
  GString *fontBuf2;
#else
  GString *tmpFileName;
  FILE *tmpFile;
#endif
  char *cffStart;
  int cffLength;
  int *cidToGIDMap;
  int nCIDs;
  SplashFontFile *ret;

#ifdef LOAD_FONTS_FROM_MEM
  if (!(ff = FoFiTrueType::make(fontBuf->getCString(), fontBuf->getLength(),
				0, gTrue))) {
#else
  if (!(ff = FoFiTrueType::load(fileName, 0, gTrue))) {
#endif
    return NULL;
  }
  cidToGIDMap = NULL;
  nCIDs = 0;
  if (ff->isHeadlessCFF()) {
    if (!ff->getCFFBlock(&cffStart, &cffLength)) {
      return NULL;
    }
#ifdef LOAD_FONTS_FROM_MEM
    fontBuf2 = new GString(cffStart, cffLength);
    if (!useCIDs) {
      cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
    }
    ret = SplashFTFontFile::loadCIDFont(this, idA, splashFontOpenTypeCFF,
					fontBuf2, cidToGIDMap, nCIDs);
    if (ret) {
      delete fontBuf;
    } else {
      delete fontBuf2;
    }
#else
    tmpFileName = NULL;
    if (!openTempFile(&tmpFileName, &tmpFile, "wb", NULL)) {
      delete ff;
      return NULL;
    }
    fwrite(cffStart, 1, cffLength, tmpFile);
    fclose(tmpFile);
    if (!useCIDs) {
      cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
    }
    ret = SplashFTFontFile::loadCIDFont(this, idA, splashFontOpenTypeCFF,
					tmpFileName->getCString(), gTrue,
					cidToGIDMap, nCIDs);
    if (ret) {
      if (deleteFile) {
	unlink(fileName);
      }
    } else {
      unlink(tmpFileName->getCString());
    }
    delete tmpFileName;
#endif
  } else {
    if (!codeToGID && !useCIDs && ff->isOpenTypeCFF()) {
      cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
    }
    ret = SplashFTFontFile::loadCIDFont(this, idA, splashFontOpenTypeCFF,
#ifdef LOAD_FONTS_FROM_MEM
					fontBuf,
#else
					fileName, deleteFile,
#endif
					codeToGID ? codeToGID : cidToGIDMap,
					codeToGID ? codeToGIDLen : nCIDs);
  }
  delete ff;
  if (!ret) {
    gfree(cidToGIDMap);
  }
  return ret;
}

SplashFontFile *SplashFTFontEngine::loadTrueTypeFont(SplashFontFileID *idA,
#ifdef LOAD_FONTS_FROM_MEM
						     GString *fontBuf,
#else
						     char *fileName,
						     GBool deleteFile,
#endif
						     int fontNum,
						     int *codeToGID,
						     int codeToGIDLen) {
  FoFiTrueType *ff;
#ifdef LOAD_FONTS_FROM_MEM
  GString *fontBuf2;
#else
  GString *tmpFileName;
  FILE *tmpFile;
#endif
  SplashFontFile *ret;

#ifdef LOAD_FONTS_FROM_MEM
  if (!(ff = FoFiTrueType::make(fontBuf->getCString(), fontBuf->getLength(),
				fontNum))) {
#else
  if (!(ff = FoFiTrueType::load(fileName, fontNum))) {
#endif
    return NULL;
  }
#ifdef LOAD_FONTS_FROM_MEM
  fontBuf2 = new GString;
  ff->writeTTF(&gstringWrite, fontBuf2);
#else
  tmpFileName = NULL;
  if (!openTempFile(&tmpFileName, &tmpFile, "wb", NULL)) {
    delete ff;
    return NULL;
  }
  ff->writeTTF(&fileWrite, tmpFile);
  fclose(tmpFile);
#endif
  delete ff;
  ret = SplashFTFontFile::loadTrueTypeFont(this, idA, splashFontTrueType,
#ifdef LOAD_FONTS_FROM_MEM
					   fontBuf2,
#else
					   tmpFileName->getCString(), gTrue,
#endif
					   0, codeToGID, codeToGIDLen);
#ifdef LOAD_FONTS_FROM_MEM
  if (ret) {
    delete fontBuf;
  } else {
    delete fontBuf2;
  }
#else
  if (ret) {
    if (deleteFile) {
      unlink(fileName);
    }
  } else {
    unlink(tmpFileName->getCString());
  }
  delete tmpFileName;
#endif
  return ret;
}

#endif /* HAVE_FREETYPE */
