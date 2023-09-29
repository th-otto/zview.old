//========================================================================
//
// OutputDev.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef OUTPUTDEV_H
#define OUTPUTDEV_H

#include <aconf.h>

#include "gtypes.h"
#include "CharTypes.h"

class GString;
class Gfx;
class GfxState;
struct GfxColor;
class GfxColorSpace;
class GfxImageColorMap;
class GfxShading;
class Stream;
class Links;
class Link;
class Catalog;
class Page;
class Function;

//------------------------------------------------------------------------
// OutputDev
//------------------------------------------------------------------------

class OutputDev {
public:

  // Constructor.
  OutputDev() {}

  // Destructor.
  virtual ~OutputDev() {}

  //----- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() = 0;

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() = 0;

  // Does this device use tilingPatternFill()?  If this returns false,
  // tiling pattern fills will be reduced to a series of other drawing
  // operations.
  virtual GBool useTilingPatternFill() { return gFalse; }

  // Does this device use drawForm()?  If this returns false,
  // form-type XObjects will be interpreted (i.e., unrolled).
  virtual GBool useDrawForm() { return gFalse; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() = 0;

  // Does this device need non-text content?
  virtual GBool needNonText() { return gTrue; }

  // Does this device require incCharCount to be called for text on
  // non-shown layers?
  virtual GBool needCharCount() { return gFalse; }



  //----- initialization and control

  // Set default transform matrix.
  virtual void setDefaultCTM(double *ctm);

  // Check to see if a page slice should be displayed.  If this
  // returns false, the page display is aborted.  Typically, an
  // OutputDev will use some alternate means to display the page
  // before returning false.
  virtual GBool checkPageSlice(Page *page, double hDPI, double vDPI,
			       int rotate, GBool useMediaBox, GBool crop,
			       int sliceX, int sliceY, int sliceW, int sliceH,
			       GBool printing,
			       GBool (*abortCheckCbk)(void *data) = NULL,
			       void *abortCheckCbkData = NULL)
    { (void) page; (void) hDPI; (void) vDPI;
      (void) rotate; (void) useMediaBox; (void) crop;
      (void) sliceX; (void) sliceY; (void) sliceW; (void) sliceH;
      (void) printing; (void) abortCheckCbk; (void) abortCheckCbkData;
      return gTrue; }

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state) { (void) pageNum; (void) state; }

  // End a page.
  virtual void endPage() {}

  //----- coordinate conversion

  // Convert between device and user coordinates.
  virtual void cvtDevToUser(double dx, double dy, double *ux, double *uy);
  virtual void cvtUserToDev(double ux, double uy, double *dx, double *dy);
  virtual void cvtUserToDev(double ux, double uy, int *dx, int *dy);

  double *getDefCTM() { return defCTM; }
  double *getDefICTM() { return defICTM; }

  //----- save/restore graphics state
  virtual void saveState(GfxState *state) {(void) state;}
  virtual void restoreState(GfxState *state) {(void) state;}

  //----- update graphics state
  virtual void updateAll(GfxState *state);
  virtual void updateCTM(GfxState *state, double m11, double m12,
			 double m21, double m22, double m31, double m32) {(void) state; (void)m11; (void)m12; (void)m21; (void)m22; (void)m31; (void)m32; }
  virtual void updateLineDash(GfxState *state) {(void) state;}
  virtual void updateFlatness(GfxState *state) {(void) state;}
  virtual void updateLineJoin(GfxState *state) {(void) state;}
  virtual void updateLineCap(GfxState *state) {(void) state;}
  virtual void updateMiterLimit(GfxState *state) {(void) state;}
  virtual void updateLineWidth(GfxState *state) {(void) state;}
  virtual void updateStrokeAdjust(GfxState *state) {(void) state;}
  virtual void updateFillColorSpace(GfxState *state) {(void) state;}
  virtual void updateStrokeColorSpace(GfxState *state) {(void) state;}
  virtual void updateFillColor(GfxState *state) {(void) state;}
  virtual void updateStrokeColor(GfxState *state) {(void) state;}
  virtual void updateBlendMode(GfxState *state) {(void) state;}
  virtual void updateFillOpacity(GfxState *state) {(void) state;}
  virtual void updateStrokeOpacity(GfxState *state) {(void) state;}
  virtual void updateFillOverprint(GfxState *state) {(void) state;}
  virtual void updateStrokeOverprint(GfxState *state) {(void) state;}
  virtual void updateOverprintMode(GfxState *state) {(void) state;}
  virtual void updateRenderingIntent(GfxState *state) {(void) state;}
  virtual void updateTransfer(GfxState *state) {(void) state;}

  //----- update text state
  virtual void updateFont(GfxState *state) {(void) state;}
  virtual void updateTextMat(GfxState *state) {(void) state;}
  virtual void updateCharSpace(GfxState *state) {(void) state;}
  virtual void updateRender(GfxState *state) {(void) state;}
  virtual void updateRise(GfxState *state) {(void) state;}
  virtual void updateWordSpace(GfxState *state) {(void) state;}
  virtual void updateHorizScaling(GfxState *state) {(void) state;}
  virtual void updateTextPos(GfxState *state) {(void) state;}
  virtual void updateTextShift(GfxState *state, double shift) {(void) state; (void)shift;}
  virtual void saveTextPos(GfxState *state) {(void) state;}
  virtual void restoreTextPos(GfxState *state) {(void) state;}

  //----- path painting
  virtual void stroke(GfxState *state) {(void) state;}
  virtual void fill(GfxState *state) {(void) state;}
  virtual void eoFill(GfxState *state) {(void) state;}
  virtual void tilingPatternFill(GfxState *state, Gfx *gfx, Object *strRef,
				 int paintType, int tilingType, Dict *resDict,
				 double *mat, double *bbox,
				 int x0, int y0, int x1, int y1,
				 double xStep, double yStep) {(void) state; (void) gfx; (void)strRef; (void) paintType; (void) tilingType;
				 (void) resDict; (void) mat; (void) bbox;
				 (void) x0; (void) y0; (void) x1; (void) y1;
				 (void) xStep; (void) yStep; }
  virtual GBool shadedFill(GfxState *state, GfxShading *shading)
    { (void) state; (void) shading; return gFalse; }

  //----- path clipping
  virtual void clip(GfxState *state) {(void) state;}
  virtual void eoClip(GfxState *state) {(void) state;}
  virtual void clipToStrokePath(GfxState *state) {(void) state;}

  //----- text drawing
  virtual void beginStringOp(GfxState *state) {(void) state;}
  virtual void endStringOp(GfxState *state) {(void) state;}
  virtual void beginString(GfxState *state, GString *s) {(void) state; (void) s;}
  virtual void endString(GfxState *state) {(void) state;}
  virtual void drawChar(GfxState *state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode code, int nBytes, Unicode *u, int uLen) {(void) state;
			(void) x; (void) y; (void) dx; (void) dy;
			(void) originX; (void) originY;
			(void) code; (void) nBytes; (void) u; (void) uLen; }
  virtual void drawString(GfxState *state, GString *s) {(void) state; (void) s;}
  virtual GBool beginType3Char(GfxState *state, double x, double y,
			       double dx, double dy,
			       CharCode code, Unicode *u, int uLen);
  virtual void endType3Char(GfxState *state) { (void) state;}
  virtual void endTextObject(GfxState *state) { (void) state;}
  virtual void incCharCount(int nChars) { (void) nChars;}
  virtual void beginActualText(GfxState *state, Unicode *u, int uLen) { (void) state; (void) u; (void) uLen; }
  virtual void endActualText(GfxState *state) { (void) state;}

  //----- image drawing
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
			     int width, int height, GBool invert,
			     GBool inlineImg, GBool interpolate);
  virtual void setSoftMaskFromImageMask(GfxState *state,
					Object *ref, Stream *str,
					int width, int height, GBool invert,
					GBool inlineImg, GBool interpolate);
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
			 int width, int height, GfxImageColorMap *colorMap,
			 int *maskColors, GBool inlineImg, GBool interpolate);
  virtual void drawMaskedImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       Object *maskRef, Stream *maskStr,
			       int maskWidth, int maskHeight,
			       GBool maskInvert, GBool interpolate);
  virtual void drawSoftMaskedImage(GfxState *state, Object *ref, Stream *str,
				   int width, int height,
				   GfxImageColorMap *colorMap,
				   Object *maskRef, Stream *maskStr,
				   int maskWidth, int maskHeight,
				   GfxImageColorMap *maskColorMap,
				   double *matte, GBool interpolate);

#ifdef OPI_SUPPORT
  //----- OPI functions
  virtual void opiBegin(GfxState *state, Dict *opiDict);
  virtual void opiEnd(GfxState *state, Dict *opiDict);
#endif

  //----- Type 3 font operators
  virtual void type3D0(GfxState *state, double wx, double wy) { (void) state; (void) wx; (void) wy;}
  virtual void type3D1(GfxState *state, double wx, double wy,
		       double llx, double lly, double urx, double ury) {(void) state;
		       (void) wx; (void) wy; (void) llx; (void) lly; (void) urx; (void) ury;}

  //----- form XObjects
  virtual void drawForm(Ref id) {(void) id;}

  //----- PostScript XObjects
  virtual void psXObject(Stream *psStream, Stream *level1Stream) {(void) psStream; (void) level1Stream; }

  //----- transparency groups and soft masks
  virtual GBool beginTransparencyGroup(GfxState *state, double *bbox,
				       GfxColorSpace *blendingColorSpace,
				       GBool isolated, GBool knockout,
				       GBool forSoftMask) { (void) state; (void) bbox; (void) blendingColorSpace; (void) isolated; (void) knockout; (void) forSoftMask; return gTrue; }
  virtual void endTransparencyGroup(GfxState *state) {(void) state;}
  virtual void paintTransparencyGroup(GfxState *state, double *bbox) {(void) state;(void) bbox;}
  virtual void setSoftMask(GfxState *state, double *bbox, GBool alpha,
			   Function *transferFunc, GfxColor *backdropColor) {(void) state;
			   (void) bbox; (void) alpha; (void) transferFunc; (void) backdropColor; }
  virtual void clearSoftMask(GfxState *state) {(void) state;}

  //----- links
  virtual void processLink(Link *link) {(void) link;}

private:

  double defCTM[6];		// default coordinate transform matrix
  double defICTM[6];		// inverse of default CTM
};

#endif
