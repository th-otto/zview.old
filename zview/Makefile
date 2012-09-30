CC				= m68k-atari-mint-gcc
CXX				= m68k-atari-mint-g++
AR 				= m68k-atari-mint-ar
RANLIB 				= m68k-atari-mint-ranlib
STACK				= m68k-atari-mint-stack
STRIP				= m68k-atari-mint-strip -s
FLAGS				= m68k-atari-mint-flags -r
DEBUG				= # -DDEBUG
COMPRESS			= upx -9
STACKSIZE			= 512k
OPTIMISATION			= -g -O2 -fomit-frame-pointer
#CPU				= -mcpu=5475
CPU				= -m68020-60
DEBUG_LIB			= # -lwout
XPDF_LIBS			= -Lpdf/fofi -lfofi -Lpdf/goo -lGoo -Lpdf/splash -lsplash
XPDF_INCS			= -Ipdf -Ipdf/goo -Ipdf/splash
LIB				= -lfreetype $(DEBUG_LIB) -lwindom1 -lldg -lgem -lz
CFLAGS				= $(DEBUG) $(CPU) $(OPTIMISATION) $(XPDF_INCS) -I../common -Wall -Wshadow -Wno-pointer-sign
CXXFLAGS			= $(DEBUG) $(CPU) $(OPTIMISATION) $(XPDF_INCS) -Wall -Wshadow

FILE_OBJ 			= file/sort.o file/file.o file/delete.o file/rename.o file/count.o
EDIT_OBJ 			= zedit/char.o zedit/edit.o zedit/zedit.o
VDI_OBJ 			= zvdi/p2c.o zvdi/color.o zvdi/dither.o zvdi/raster.o zvdi/raster_resize.o zvdi/pixel.o zvdi/vdi.o
CATALOG_OBJ			= catalog/catalog_slider.o catalog/catalog_size.o catalog/catalog_other_event.o	\
				  catalog/catalog_keyb.o catalog/catalog_mouse.o catalog/catalog_iconify.o 		\
				  catalog/catalog_icons.o catalog/catalog_entry.o catalog/catalog_mini_entry.o 	\
				  catalog/catalog_popup.o catalog/catalog.o
PDF_OBJ 			= pdf/xpdf/Annot.o pdf/xpdf/Array.o pdf/xpdf/BuiltinFont.o pdf/xpdf/BuiltinFontTables.o \
				  pdf/xpdf/Catalog.o pdf/xpdf/CharCodeToUnicode.o pdf/xpdf/CMap.o pdf/xpdf/Decrypt.o pdf/xpdf/Dict.o pdf/xpdf/Error.o \
				  pdf/xpdf/FontEncodingTables.o pdf/xpdf/Function.o pdf/xpdf/Gfx.o pdf/xpdf/GfxFont.o pdf/xpdf/GfxState.o \
				  pdf/xpdf/GlobalParams.o pdf/xpdf/JArithmeticDecoder.o pdf/xpdf/JBIG2Stream.o pdf/xpdf/JPXStream.o \
				  pdf/xpdf/Lexer.o pdf/xpdf/Link.o pdf/xpdf/NameToCharCode.o pdf/xpdf/Object.o pdf/xpdf/Outline.o pdf/xpdf/OutputDev.o \
				  pdf/xpdf/Page.o pdf/xpdf/Parser.o pdf/xpdf/SecurityHandler.o pdf/xpdf/PDFDoc.o pdf/xpdf/PDFDocEncoding.o pdf/xpdf/PSTokenizer.o \
				  pdf/xpdf/SplashOutputDev.o pdf/xpdf/Stream.o pdf/xpdf/TextOutputDev.o pdf/xpdf/UnicodeMap.o \
				  pdf/xpdf/UnicodeTypeTable.o pdf/xpdf/XRef.o pdf/pdflib.o pdf/xpdf/OptionalContent.o

OBJ 				= $(PDF_OBJ) $(CATALOG_OBJ) $(FILE_OBJ) $(EDIT_OBJ) $(VDI_OBJ) \
				  custom_font.o infobox.o pref_dialog.o zaes.o wintimer.o av_prot.o chrono.o plugins.o mfdb.o 	\
				  txt_data.o pdf_load.o pic_load.o pic_save.o pic_resize.o resample.o save_dialog.o menu.o prefs.o full_scr.o winpdf.o winimg.o	\
				  debug.o progress.o ztext.o close_modal.o jpg_dialog.o tiff_dialog.o main.o

PROGRAM 			= zview.app


all: pdf $(PROGRAM) stack strip #compress

$(PROGRAM): $(OBJ)
	$(CXX) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) $(XPDF_LIBS) $(LIB)


XPDF_VERSION	= 3.03
xpdf-${XPDF_VERSION}.tar.gz:
	wget ftp://ftp.foolabs.com/pub/xpdf/xpdf-${XPDF_VERSION}.tar.gz

pdf: xpdf-${XPDF_VERSION}.tar.gz
	tar xzf xpdf-${XPDF_VERSION}.tar.gz && mv xpdf-${XPDF_VERSION} pdf && \
	cd $@ && \
	cat ../howto_build/xpdf-${XPDF_VERSION}-zview.patch | patch -p1 && \
	CFLAGS='-O2 -fomit-frame-pointer $(CPU)' CXXFLAGS='-O2 -fomit-frame-pointer $(CPU)' ./configure --with-freetype2-includes=$$HOME/gnu-tools/m68k-atari-mint/include/freetype2 --without-x --host=m68k-atari-mint && \
	make && \
	cd .. && touch $@

strip:
	$(STRIP) $(PROGRAM)

stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)
	$(FLAGS) $(PROGRAM)

clean_zview:
	rm -f *~ *.o *.app zvdi/*.o zedit/*.o file/*.o catalog/*.o
	
clean_pdf:
	make -C pdf clean && rm pdf/*.o

clean: clean_zview
