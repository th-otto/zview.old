USE_WINDOM2=0
top_srcdir=..
include $(top_srcdir)/configvars

DEBUG_LIB			= # -lwout

ZVPDF_SLB = 1

XPDF_CPPFLAGS                   := -DUSE_PRIVATE_FONTDIR \
				    -I$(shell dirname $(dir $(shell $(CC) --print-file-name=libc.a)))/include/freetype2 \
				    -DFREETYPE_SLB -DPNGLIB_SLB -DZLIB_SLB -DBZIP2_SLB
ifeq ($(ZVPDF_SLB),1)
XPDF_CPPFLAGS +=		-DZVPDF_SLB
endif

XPDF_INCS			= -Ixpdf -Ixpdf/fofi -Ixpdf/goo -Ixpdf/splash $(XPDF_CPPFLAGS)
LIB				= $(DEBUG_LIB) $(WINDOM_LIBS) -lldg -lgem -lfreetype.slb
                                # -lpng.slb -lbz2.slb -lz.slb
CFLAGS				+= $(XPDF_INCS) -I../common
CXXFLAGS			+= $(XPDF_INCS) -I../common

FILE_OBJ 			= file/sort.o file/file.o file/delete.o file/rename.o file/count.o
EDIT_OBJ 			= zedit/char.o zedit/edit.o zedit/zedit.o
VDI_OBJ 			= zvdi/p2c.o zvdi/color.o zvdi/raster.o zvdi/raster_resize.o zvdi/pixel.o zvdi/vdi.o
CATALOG_OBJ			= catalog/catalog_slider.o catalog/catalog_size.o catalog/catalog_other_event.o	\
				  catalog/catalog_keyb.o catalog/catalog_mouse.o catalog/catalog_iconify.o 		\
				  catalog/catalog_icons.o catalog/catalog_entry.o catalog/catalog_mini_entry.o 	\
				  catalog/catalog_popup.o catalog/catalog.o

OBJ 				= $(CATALOG_OBJ) $(FILE_OBJ) $(EDIT_OBJ) $(VDI_OBJ) \
				  custom_font.o infobox.o pref_dialog.o zaes.o wintimer.o av_prot.o chrono.o plugins.o mfdb.o 	\
				  txt_data.o pdf_load.o pic_load.o pic_save.o pic_resize.o resample.o save_dialog.o menu.o prefs.o full_scr.o winpdf.o winimg.o	\
				  debug.o progress.o ztext.o close_modal.o jpg_dialog.o tiff_dialog.o main.o \
				  plugins/common/slbload.o plugins/common/slbexec.o plugins/common/zvplugin.o

#
# These are currently needed because we cannot link to zlib.slb.a etc,
# because that would pull in the import functions for the shared library,
# and would also be used by the pdf library, which is not yet prepared for that
#
OBJ +=	plugins/slb/zlib_open.o plugins/slb/png_open.o plugins/slb/lzma_open.o plugins/slb/jpeg_open.o plugins/slb/tiff_open.o plugins/slb/exif_open.o plugins/slb/bzip2_open.o

PROGRAM 			= zview.app

all:: $(PROGRAM)
	$(MAKE) -C plugins

##############################################################################

XPDF_VERSION			= 4.01.01

PDF_OBJ				= xpdf/xpdf/Annot.o xpdf/xpdf/Array.o xpdf/xpdf/BuiltinFont.o xpdf/xpdf/BuiltinFontTables.o \
                                  xpdf/xpdf/Catalog.o xpdf/xpdf/CharCodeToUnicode.o xpdf/xpdf/CMap.o xpdf/xpdf/Decrypt.o xpdf/xpdf/Dict.o xpdf/xpdf/Error.o \
                                  xpdf/xpdf/FontEncodingTables.o xpdf/xpdf/Function.o xpdf/xpdf/Gfx.o xpdf/xpdf/GfxFont.o xpdf/xpdf/GfxState.o \
                                  xpdf/xpdf/GlobalParams.o xpdf/xpdf/JArithmeticDecoder.o xpdf/xpdf/JBIG2Stream.o xpdf/xpdf/JPXStream.o \
                                  xpdf/xpdf/Lexer.o xpdf/xpdf/Link.o xpdf/xpdf/NameToCharCode.o xpdf/xpdf/Object.o xpdf/xpdf/Outline.o xpdf/xpdf/OutputDev.o \
                                  xpdf/xpdf/Page.o xpdf/xpdf/Parser.o xpdf/xpdf/SecurityHandler.o xpdf/xpdf/PDFDoc.o xpdf/xpdf/PDFDocEncoding.o xpdf/xpdf/PSTokenizer.o \
                                  xpdf/xpdf/SplashOutputDev.o xpdf/xpdf/Stream.o xpdf/xpdf/TextOutputDev.o xpdf/xpdf/TextString.o xpdf/xpdf/UnicodeMap.o \
                                  xpdf/xpdf/UnicodeTypeTable.o xpdf/xpdf/XRef.o xpdf/xpdf/OptionalContent.o \
				  xpdf/fofi/FoFiBase.o xpdf/fofi/FoFiEncodings.o xpdf/fofi/FoFiIdentifier.o \
				  xpdf/fofi/FoFiTrueType.o xpdf/fofi/FoFiType1.o xpdf/fofi/FoFiType1C.o \
				  xpdf/goo/FixedPoint.o xpdf/goo/GHash.o xpdf/goo/GList.o xpdf/goo/GString.o \
				  xpdf/goo/gfile.o xpdf/goo/gmem.o xpdf/goo/gmempp.o xpdf/goo/parseargs.o \
				  xpdf/splash/Splash.o xpdf/splash/SplashBitmap.o xpdf/splash/SplashClip.o \
				  xpdf/splash/SplashFTFont.o xpdf/splash/SplashFTFontEngine.o xpdf/splash/SplashFTFontFile.o \
				  xpdf/splash/SplashFont.o xpdf/splash/SplashFontEngine.o xpdf/splash/SplashFontFile.o \
				  xpdf/splash/SplashFontFileID.o xpdf/splash/SplashPath.o xpdf/splash/SplashPattern.o \
				  xpdf/splash/SplashScreen.o xpdf/splash/SplashState.o xpdf/splash/SplashXPath.o xpdf/splash/SplashXPathScanner.o

ifeq ($(firstword $(subst ., ,$(XPDF_VERSION))),4)
PDF_OBJ				+= xpdf/xpdf/PDF417Barcode.o
PDF_OBJ				+= xpdf/xpdf/UTF8.o
PDF_OBJ				+= xpdf/xpdf/UnicodeRemapping.o
endif

xpdf-${XPDF_VERSION}.tar.gz:
	: wget ftp://ftp.foolabs.com/pub/xpdf/xpdf-${XPDF_VERSION}.tar.gz
	wget https://xpdfreader-dl.s3.amazonaws.com/old/xpdf-${XPDF_VERSION}.tar.gz

$(PDF_OBJ): xpdf/config.status

xpdf/config.status:
	test -d xpdf || exit 1 && \
	cd xpdf && \
	export PKG_CONFIG_LIBDIR=/usr/m68k-atari-mint/lib/pkgconfig && \
	export PKG_CONFIG_PATH=/usr/m68k-atari-mint/lib/pkgconfig && \
	CFLAGS='$(OPTIMISATION) $(CPU) $(XPDF_CPPFLAGS)' CXXFLAGS='$(OPTIMISATION) $(CPU) $(XPDF_CPPFLAGS)' ./configure --without-x --host=m68k-atari-mint
	
##############################################################################

include $(top_srcdir)/rules

##############################################################################

libpdf.a: $(PDF_OBJ)
	$(RM) $@
	$(AR) rcs $@ $(PDF_OBJ)

ifeq ($(ZVPDF_SLB),1)
$(PROGRAM): $(OBJ) pdflib/zvpdf_load.o slb/$(SLB_SUBDIR)zvpdf.slb
	$(CC) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) pdflib/zvpdf_load.o $(LDFLAGS) -Wl,--stack,512k $(LIB) -lm -Wl,-Map,zview.map
else
$(PROGRAM): $(OBJ) pdflib/pdflib.o libpdf.a
	$(CXX) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) pdflib/pdflib.o libpdf.a $(LDFLAGS) -Wl,--stack,512k $(LIB) -Wl,-Map,zview.map
endif
	$(COMPRESS) $(PROGRAM)

slb/$(SLB_SUBDIR)zvpdf.slb: pdflib/header.lo pdflib/zvpdf.lo pdflib/pdflib.lo libpdf.a
	$(CXX) -nostartfiles -Wl,--mprg-flags,0x0f -Wl,--stack,1k $(CXXFLAGS) -o $@ $^ -lfreetype.slb $(LDFLAGS) -nostdlib -lgcc -lstdc++ -lm -lgcc

pdflib/header.lo: plugins/common/plugin.h plugins/common/zview.h pdflib/exports.h

##############################################################################

clean_zview:
	rm -f *~ *.o *.app zvdi/*.o zedit/*.o file/*.o catalog/*.o pdflib/*.o pdflib/*.lo slb/*/*.slb xpdf*/*/*.o
	$(MAKE) -C plugins clean

clean_pdf:
	rm -f pdflib/*.o slb/*/*.slb $(PDF_OBJ) libpdf.a

clean: clean_zview
