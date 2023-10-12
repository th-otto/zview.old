noinst_LIBRARIES = libxpdf.a libxpdf_widget.a

bin_PROGRAMS = pdftops pdftotext pdfinfo pdffonts pdfdetach pdfimages
if HAVE_LIBPNG
if HAVE_FREETYPE
bin_PROGRAMS += pdftohtml pdftopng
endif
endif
if HAVE_FREETYPE
bin_PROGRAMS += pdftoppm
endif

AM_CPPFLAGS = -I$(top_builddir) -I$(srcdir) -I$(top_srcdir)/goo -I$(top_srcdir)/fofi -I$(top_srcdir)/splash
AM_CPPFLAGS += $(PNG_CFLAGS)
AM_CPPFLAGS += $(LCMS_CFLAGS)
AM_CPPFLAGS += $(FREETYPE2_CFLAGS)

libxpdf_a_SOURCES = \
	config.h \
	AcroForm.cc AcroForm.h \
	Annot.cc Annot.h \
	Array.cc Array.h \
	BuiltinFont.cc BuiltinFont.h \
	BuiltinFontTables.cc BuiltinFontTables.h \
	CMap.cc CMap.h \
	Catalog.cc Catalog.h \
	CharCodeToUnicode.cc CharCodeToUnicode.h \
	CharTypes.h \
	CompactFontTables.h \
	Decrypt.cc Decrypt.h \
	Dict.cc Dict.h \
	Error.cc Error.h ErrorCodes.h \
	FontEncodingTables.cc FontEncodingTables.h \
	Function.cc Function.h \
	Gfx.cc Gfx.h \
	GfxFont.cc GfxFont.h \
	GfxState.cc GfxState.h \
	GlobalParams.cc GlobalParams.h \
	JArithmeticDecoder.cc JArithmeticDecoder.h \
	JBIG2Stream.cc JBIG2Stream.h \
	JPXStream.cc JPXStream.h \
	Lexer.cc Lexer.h \
	Link.cc Link.h \
	NameToCharCode.cc NameToCharCode.h NameToUnicodeTable.h \
	Object.cc Object.h \
	OptionalContent.cc OptionalContent.h \
	Outline.cc Outline.h \
	OutputDev.cc OutputDev.h \
	PDF417Barcode.cc PDF417Barcode.h \
	PDFDoc.cc PDFDoc.h \
	PDFDocEncoding.cc PDFDocEncoding.h \
	PSTokenizer.cc PSTokenizer.h \
	Page.cc Page.h \
	Parser.cc Parser.h \
	SecurityHandler.cc SecurityHandler.h \
	Stream.cc Stream.h Stream-CCITT.h \
	TextOutputDev.cc TextOutputDev.h \
	TextString.cc TextString.h \
	UTF8.cc UTF8.h \
	UnicodeMap.cc UnicodeMap.h UnicodeMapTables.h \
	UnicodeRemapping.cc UnicodeRemapping.h \
	UnicodeTypeTable.cc UnicodeTypeTable.h \
	XFAScanner.cc XFAScanner.h \
	XRef.cc XRef.h \
	Zoox.cc Zoox.h \
	$(empty)

if HAVE_LCMS
libxpdf_a_SOURCES += ColorManager.cc ColorManager.h
endif


libxpdf_widget_a_SOURCES =
if HAVE_QT
if MULTITHREADED
if USE_EXCEPTIONS
libxpdf_widget_a_SOURCES += \
	DisplayState.cc DisplayState.h \
	PDFCore.cc PDFCore.h \
	PreScanOutputDev.cc PreScanOutputDev.h \
	TileCache.cc TileCache.h \
	TileCompositor.cc TileCompositor.h \
	TileMap.cc TileMap.h \
	$(empty)
if HAVE_FREETYPE
libxpdf_widget_a_SOURCES += \
	PSOutputDev.cc PSOutputDev.h \
	SplashOutputDev.cc SplashOutputDev.h \
	ShadingImage.cc ShadingImage.h \
	$(empty)
endif
if HIGHLIGHTED_REGIONS
libxpdf_widget_a_SOURCES += HighlightFile.cc HighlightFile.h
endif
if XPDFWIDGET_PRINTING
if OS_WIN32
libxpdf_widget_a_SOURCES += GDIOutputDev.cc GDIOutputDev.h
libxpdf_widget_a_SOURCES += WinPDFPrinter.cc WinPDFPrinter.h
endif
endif
endif
endif
endif

pdftops_SOURCES = pdftops.cc \
	PreScanOutputDev.cc PreScanOutputDev.h \
	PSOutputDev.cc PSOutputDev.h \
	SplashOutputDev.cc SplashOutputDev.h \
	ShadingImage.cc ShadingImage.h \
	$(empty)
pdftops_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi
pdftops_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a
if HAVE_FREETYPE
pdftops_LDADD += -L../splash -lsplash
pdftops_DEPENDENCIES += ../splash/libsplash.a
endif
pdftops_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS) $(PNG_LIBS)

pdffonts_SOURCES = pdffonts.cc
pdffonts_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi
pdffonts_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a
pdffonts_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)

pdfdetach_SOURCES = pdfdetach.cc
pdfdetach_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi
pdfdetach_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a
pdfdetach_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)

pdftoppm_SOURCES = pdftoppm.cc \
	SplashOutputDev.cc SplashOutputDev.h \
	ShadingImage.cc ShadingImage.h
pdftoppm_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi -L../splash -lsplash
pdftoppm_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a ../splash/libsplash.a
pdftoppm_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)

pdftopng_SOURCES = pdftopng.cc \
	SplashOutputDev.cc SplashOutputDev.h \
	ShadingImage.cc ShadingImage.h
pdftopng_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi -L../splash -lsplash
pdftopng_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a ../splash/libsplash.a
pdftopng_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS) $(PNG_LIBS)

pdfinfo_SOURCES = pdfinfo.cc
pdfinfo_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi
pdfinfo_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a
pdfinfo_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)

pdftohtml_SOURCES = pdftohtml.cc HTMLGen.cc HTMLGen.h \
	SplashOutputDev.cc SplashOutputDev.h \
	ShadingImage.cc ShadingImage.h \
	WebFont.cc WebFont.h
pdftohtml_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi -L../splash -lsplash
pdftohtml_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a ../splash/libsplash.a
pdftohtml_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS) $(PNG_LIBS)

pdftotext_SOURCES = pdftotext.cc
pdftotext_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi -L../splash -lsplash
pdftotext_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a ../splash/libsplash.a
pdftotext_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)

pdfimages_SOURCES = pdfimages.cc ImageOutputDev.cc ImageOutputDev.h
pdfimages_LDADD = -L. -lxpdf -L../goo -lgoo -L../fofi -lfofi
pdfimages_DEPENDENCIES = libxpdf.a ../goo/libgoo.a ../fofi/libfofi.a
pdfimages_LDADD += $(LCMS_LIBS) $(FREETYPE2_LIBS)
