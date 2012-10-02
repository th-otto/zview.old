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
XPDF_LIBS			= -Lxpdf-${XPDF_VERSION}/fofi -lfofi -Lxpdf-${XPDF_VERSION}/goo -lGoo -Lxpdf-${XPDF_VERSION}/splash -lsplash
XPDF_INCS			= -Ixpdf-${XPDF_VERSION} -Ixpdf-${XPDF_VERSION}/goo -Ixpdf-${XPDF_VERSION}/splash
LIB				= -lfreetype $(DEBUG_LIB) -lwindom1 -lldg -lgem -lz
CFLAGS				= $(DEBUG) $(CPU) $(OPTIMISATION) $(XPDF_INCS) -I../common -Wall -Wshadow -Wno-pointer-sign
CXXFLAGS			= $(DEBUG) $(CPU) $(OPTIMISATION) $(XPDF_INCS) -I../common -Wall -Wshadow

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
				  debug.o progress.o ztext.o close_modal.o jpg_dialog.o tiff_dialog.o main.o

PROGRAM 			= zview.app
PDF_LIB				= libpdf.a

all: $(PROGRAM) stack strip #compress

$(PROGRAM): $(PDF_LIB) $(OBJ)
	$(CXX) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) -L. -lpdf $(XPDF_LIBS) $(LIB)

##############################################################################

XPDF_VERSION			= 3.03

PDF_OBJ				= xpdf-${XPDF_VERSION}/xpdf/Annot.o xpdf-${XPDF_VERSION}/xpdf/Array.o xpdf-${XPDF_VERSION}/xpdf/BuiltinFont.o xpdf-${XPDF_VERSION}/xpdf/BuiltinFontTables.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Catalog.o xpdf-${XPDF_VERSION}/xpdf/CharCodeToUnicode.o xpdf-${XPDF_VERSION}/xpdf/CMap.o xpdf-${XPDF_VERSION}/xpdf/Decrypt.o xpdf-${XPDF_VERSION}/xpdf/Dict.o xpdf-${XPDF_VERSION}/xpdf/Error.o \
                                  xpdf-${XPDF_VERSION}/xpdf/FontEncodingTables.o xpdf-${XPDF_VERSION}/xpdf/Function.o xpdf-${XPDF_VERSION}/xpdf/Gfx.o xpdf-${XPDF_VERSION}/xpdf/GfxFont.o xpdf-${XPDF_VERSION}/xpdf/GfxState.o \
                                  xpdf-${XPDF_VERSION}/xpdf/GlobalParams.o xpdf-${XPDF_VERSION}/xpdf/JArithmeticDecoder.o xpdf-${XPDF_VERSION}/xpdf/JBIG2Stream.o xpdf-${XPDF_VERSION}/xpdf/JPXStream.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Lexer.o xpdf-${XPDF_VERSION}/xpdf/Link.o xpdf-${XPDF_VERSION}/xpdf/NameToCharCode.o xpdf-${XPDF_VERSION}/xpdf/Object.o xpdf-${XPDF_VERSION}/xpdf/Outline.o xpdf-${XPDF_VERSION}/xpdf/OutputDev.o \
                                  xpdf-${XPDF_VERSION}/xpdf/Page.o xpdf-${XPDF_VERSION}/xpdf/Parser.o xpdf-${XPDF_VERSION}/xpdf/SecurityHandler.o xpdf-${XPDF_VERSION}/xpdf/PDFDoc.o xpdf-${XPDF_VERSION}/xpdf/PDFDocEncoding.o xpdf-${XPDF_VERSION}/xpdf/PSTokenizer.o \
                                  xpdf-${XPDF_VERSION}/xpdf/SplashOutputDev.o xpdf-${XPDF_VERSION}/xpdf/Stream.o xpdf-${XPDF_VERSION}/xpdf/TextOutputDev.o xpdf-${XPDF_VERSION}/xpdf/UnicodeMap.o \
                                  xpdf-${XPDF_VERSION}/xpdf/UnicodeTypeTable.o xpdf-${XPDF_VERSION}/xpdf/XRef.o xpdf-${XPDF_VERSION}/xpdf/OptionalContent.o pdflib.o
                                  
xpdf-${XPDF_VERSION}.tar.gz:
	wget ftp://ftp.foolabs.com/pub/xpdf/xpdf-${XPDF_VERSION}.tar.gz
	
xpdf-${XPDF_VERSION}: xpdf-${XPDF_VERSION}.tar.gz
	tar xzf xpdf-${XPDF_VERSION}.tar.gz && \
	cd $@ && \
	cat ../howto_build/xpdf-${XPDF_VERSION}-zview.patch | patch -p1 && \
	CFLAGS='-O2 -fomit-frame-pointer $(CPU)' CXXFLAGS='-O2 -fomit-frame-pointer $(CPU)' ./configure --with-freetype2-includes=$$HOME/gnu-tools/m68k-atari-mint/include/freetype2 --without-x --host=m68k-atari-mint && \
	make && \
	cd .. && \
	touch $@
	
$(PDF_LIB): xpdf-${XPDF_VERSION} $(PDF_OBJ)
	rm -f $@
	$(AR) cru $@ $(PDF_OBJ)

##############################################################################
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
	rm -rf xpdf-${XPDF_VERSION} $(PDF_LIB)

clean: clean_zview
