#========================================================================
#
# xpdf-qt/Makefile.am
#
# Automake script for the xpdf application.
#
# Copyright 2019 Thorsten Otto
#
#========================================================================

if HAVE_QT
if HAVE_FREETYPE
if MULTITHREADED
if USE_EXCEPTIONS
bin_PROGRAMS = xpdf
endif
endif
endif
endif

AM_CPPFLAGS = -I$(top_builddir) -I$(srcdir) -I$(top_srcdir)/goo -I$(top_srcdir)/fofi -I$(top_srcdir)/splash -I$(top_srcdir)/xpdf
AM_CPPFLAGS += $(PNG_CFLAGS)
AM_CPPFLAGS += $(LCMS_CFLAGS)
AM_CPPFLAGS += $(FREETYPE2_CFLAGS)
AM_CPPFLAGS += $(QT_CFLAGS)
AM_CPPFLAGS += $(PTHREAD_CFLAGS)

MOC_SOURCES = moc_XpdfApp.cc moc_XpdfViewer.cc moc_XpdfWidget.cc

xpdf_SOURCES = \
	xpdf.cc \
	QtPDFCore.cc QtPDFCore.h \
	XpdfApp.cc XpdfApp.h \
	XpdfViewer.cc XpdfViewer.h \
	XpdfWidget.cc XpdfWidget.h \
	$(MOC_SOURCES) \
	qrc_icons.cc \
	$(empty)

QRC_ICONS = \
	back.svg \
	findNext.svg \
	findPrevious.svg \
	findSettings.svg \
	fitPage.svg \
	fitPageOn.svg \
	fitWidth.svg \
	fitWidthOn.svg \
	forward.svg \
	indicator-icon0.svg \
	indicator-icon1.svg \
	indicator-icon2.svg \
	indicator-icon3.svg \
	indicator-icon4.svg \
	indicator-icon5.svg \
	indicator-icon6.svg \
	indicator-icon7.svg \
	indicator-icon-err0.svg \
	indicator-icon-err1.svg \
	indicator-icon-err2.svg \
	indicator-icon-err3.svg \
	indicator-icon-err4.svg \
	indicator-icon-err5.svg \
	indicator-icon-err6.svg \
	indicator-icon-err7.svg \
	selectModeLinear.svg \
	selectModeBlock.svg \
	xpdf-icon.svg \
	zoomIn.svg \
	zoomOut.svg \
	$(empty)

if XPDFWIDGET_PRINTING
xpdf_SOURCES += XpdfWidgetPrint.cc XpdfWidgetPrint.h
endif

xpdf_LDADD = -L../xpdf -lxpdf_widget -lxpdf -L../goo -lgoo -L../fofi -lfofi -L../splash -lsplash
xpdf_DEPENDENCIES = ../xpdf/libxpdf.a ../xpdf/libxpdf_widget.a ../goo/libgoo.a ../fofi/libfofi.a ../splash/libsplash.a
xpdf_LDADD += $(QT_LIBS) $(LCMS_LIBS) $(FREETYPE2_LIBS) $(PTHREAD_CFLAGS) $(PTHREAD_LIBS)

moc_XpdfApp.cc: XpdfApp.h
	$(AM_V_GEN)$(MOC) -o$@ $(QT_DEFS) $<

moc_XpdfViewer.cc: XpdfViewer.h
	$(AM_V_GEN)$(MOC) -o$@ $(QT_DEFS) $<

moc_XpdfWidget.cc: XpdfWidget.h
	$(AM_V_GEN)$(MOC) -o$@ $(QT_DEFS) $<

qrc_icons.cc: icons.qrc $(QRC_ICONS)
	$(AM_V_GEN)$(RCC) --name icons -o $@ $<

CLEANFILES = $(MOC_SOURCES) $(MOC_SOURCES:cpp=cpp_parameters) qrc_icons.cc

EXTRA_DIST = icons.qrc $(QRC_ICONS) xpdf.rc xpdf-icon.ico
