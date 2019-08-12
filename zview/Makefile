USE_WINDOM2=0
top_srcdir=..
include $(top_srcdir)/configvars

DEBUG_LIB			= # -lwout

XPDF_LIBS			= 
XPDF_INCS			= -Ixpdf-${XPDF_VERSION} -Ixpdf-${XPDF_VERSION}/fofi -Ixpdf-${XPDF_VERSION}/goo -Ixpdf-${XPDF_VERSION}/splash -DUSE_PRIVATE_FONTDIR
LIB				= -lfreetype $(DEBUG_LIB) $(WINDOM_LIBS) -lldg -lgem -lpng -lz
CFLAGS				+= $(XPDF_INCS) -I../common
CXXFLAGS			+= $(XPDF_INCS) -I../common

FILE_OBJ 			= file/sort.o file/file.o file/delete.o file/rename.o file/count.o
EDIT_OBJ 			= zedit/char.o zedit/edit.o zedit/zedit.o
VDI_OBJ 			= zvdi/p2c.o zvdi/color.o zvdi/dither.o zvdi/raster.o zvdi/raster_resize.o zvdi/pixel.o zvdi/vdi.o
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
OBJ +=	plugins/slb/zlib_open.o plugins/slb/png_open.o plugins/slb/lzma_open.o plugins/slb/jpeg_open.o plugins/slb/tiff_open.o plugins/slb/exif_open.o

PROGRAM 			= zview.app
PDF_LIB				= libpdf.a

all:: $(PROGRAM)
	$(MAKE) -C plugins

$(PROGRAM): $(PDF_LIB) $(OBJ)
	$(CXX) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,512k -L. -lpdf $(XPDF_LIBS) $(LIB)
	$(COMPRESS) $(PROGRAM)

##############################################################################

XPDF_VERSION			= 4.01.01

PDF_OBJ				= xpdf-${XPDF_VERSION}/xpdf/Annot.o xpdf-${XPDF_VERSION}/xpdf/Array.o xpdf-${XPDF_VERSION}/xpdf/BuiltinFont.o xpdf-${XPDF_VERSION}/xpdf/BuiltinFontTables.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Catalog.o xpdf-${XPDF_VERSION}/xpdf/CharCodeToUnicode.o xpdf-${XPDF_VERSION}/xpdf/CMap.o xpdf-${XPDF_VERSION}/xpdf/Decrypt.o xpdf-${XPDF_VERSION}/xpdf/Dict.o xpdf-${XPDF_VERSION}/xpdf/Error.o \
                                  xpdf-${XPDF_VERSION}/xpdf/FontEncodingTables.o xpdf-${XPDF_VERSION}/xpdf/Function.o xpdf-${XPDF_VERSION}/xpdf/Gfx.o xpdf-${XPDF_VERSION}/xpdf/GfxFont.o xpdf-${XPDF_VERSION}/xpdf/GfxState.o \
                                  xpdf-${XPDF_VERSION}/xpdf/GlobalParams.o xpdf-${XPDF_VERSION}/xpdf/JArithmeticDecoder.o xpdf-${XPDF_VERSION}/xpdf/JBIG2Stream.o xpdf-${XPDF_VERSION}/xpdf/JPXStream.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Lexer.o xpdf-${XPDF_VERSION}/xpdf/Link.o xpdf-${XPDF_VERSION}/xpdf/NameToCharCode.o xpdf-${XPDF_VERSION}/xpdf/Object.o xpdf-${XPDF_VERSION}/xpdf/Outline.o xpdf-${XPDF_VERSION}/xpdf/OutputDev.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Page.o xpdf-${XPDF_VERSION}/xpdf/Parser.o xpdf-${XPDF_VERSION}/xpdf/SecurityHandler.o xpdf-${XPDF_VERSION}/xpdf/PDFDoc.o xpdf-${XPDF_VERSION}/xpdf/PDFDocEncoding.o xpdf-${XPDF_VERSION}/xpdf/PSTokenizer.o \
                                  xpdf-${XPDF_VERSION}/xpdf/SplashOutputDev.o xpdf-${XPDF_VERSION}/xpdf/Stream.o xpdf-${XPDF_VERSION}/xpdf/TextOutputDev.o xpdf-${XPDF_VERSION}/xpdf/TextString.o xpdf-${XPDF_VERSION}/xpdf/UnicodeMap.o \
                                  xpdf-${XPDF_VERSION}/xpdf/UnicodeTypeTable.o xpdf-${XPDF_VERSION}/xpdf/XRef.o xpdf-${XPDF_VERSION}/xpdf/OptionalContent.o \
				  xpdf-${XPDF_VERSION}/fofi/FoFiBase.o xpdf-${XPDF_VERSION}/fofi/FoFiEncodings.o xpdf-${XPDF_VERSION}/fofi/FoFiIdentifier.o \
				  xpdf-${XPDF_VERSION}/fofi/FoFiTrueType.o xpdf-${XPDF_VERSION}/fofi/FoFiType1.o xpdf-${XPDF_VERSION}/fofi/FoFiType1C.o \
				  xpdf-${XPDF_VERSION}/goo/FixedPoint.o xpdf-${XPDF_VERSION}/goo/GHash.o xpdf-${XPDF_VERSION}/goo/GList.o xpdf-${XPDF_VERSION}/goo/GString.o \
				  xpdf-${XPDF_VERSION}/goo/gfile.o xpdf-${XPDF_VERSION}/goo/gmem.o xpdf-${XPDF_VERSION}/goo/gmempp.o xpdf-${XPDF_VERSION}/goo/parseargs.o \
				  xpdf-${XPDF_VERSION}/splash/Splash.o xpdf-${XPDF_VERSION}/splash/SplashBitmap.o xpdf-${XPDF_VERSION}/splash/SplashClip.o \
				  xpdf-${XPDF_VERSION}/splash/SplashFTFont.o xpdf-${XPDF_VERSION}/splash/SplashFTFontEngine.o xpdf-${XPDF_VERSION}/splash/SplashFTFontFile.o \
				  xpdf-${XPDF_VERSION}/splash/SplashFont.o xpdf-${XPDF_VERSION}/splash/SplashFontEngine.o xpdf-${XPDF_VERSION}/splash/SplashFontFile.o \
				  xpdf-${XPDF_VERSION}/splash/SplashFontFileID.o xpdf-${XPDF_VERSION}/splash/SplashPath.o xpdf-${XPDF_VERSION}/splash/SplashPattern.o \
				  xpdf-${XPDF_VERSION}/splash/SplashScreen.o xpdf-${XPDF_VERSION}/splash/SplashState.o xpdf-${XPDF_VERSION}/splash/SplashXPath.o xpdf-${XPDF_VERSION}/splash/SplashXPathScanner.o \
                                  pdflib.o

ifeq ($(firstword $(subst ., ,$(XPDF_VERSION))),4)
PDF_OBJ				+= xpdf-${XPDF_VERSION}/xpdf/PDF417Barcode.o
PDF_OBJ				+= xpdf-${XPDF_VERSION}/xpdf/UTF8.o
PDF_OBJ				+= xpdf-${XPDF_VERSION}/xpdf/UnicodeRemapping.o
endif

xpdf-${XPDF_VERSION}.tar.gz:
	: wget ftp://ftp.foolabs.com/pub/xpdf/xpdf-${XPDF_VERSION}.tar.gz
	wget https://xpdfreader-dl.s3.amazonaws.com/old/xpdf-${XPDF_VERSION}.tar.gz

$(PDF_OBJ): xpdf-${XPDF_VERSION}/config.status

xpdf-${XPDF_VERSION}/config.status:
	test -d xpdf-${XPDF_VERSION} || tar xzf xpdf-${XPDF_VERSION}.tar.gz && \
	cd xpdf-${XPDF_VERSION} && \
	cat ../howto_build/xpdf-${XPDF_VERSION}-zview.patch | patch -p1 && \
	export PKG_CONFIG_LIBDIR=/usr/m68k-atari-mint/lib/pkg-config && \
	export PKG_CONFIG_PATH=/usr/m68k-atari-mint/lib/pkg-config && \
	CFLAGS='-O2 -fomit-frame-pointer $(CPU)' CXXFLAGS='-O2 -fomit-frame-pointer $(CPU)' ./configure --without-x --host=m68k-atari-mint && \
	$(MAKE)
	
$(PDF_LIB): xpdf-${XPDF_VERSION} $(PDF_OBJ)
	rm -f $@
	$(AR) cru $@ $(PDF_OBJ)

##############################################################################

include $(top_srcdir)/rules

clean_zview:
	rm -f *~ *.o *.app zvdi/*.o zedit/*.o file/*.o catalog/*.o xpdf-*/*/*.o
	$(MAKE) -C plugins clean

clean_pdf:
	rm -rf xpdf-${XPDF_VERSION} $(PDF_LIB)

clean: clean_zview
