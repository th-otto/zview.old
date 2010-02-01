CC				= gcc3
STRIP			= strip
STACK			= stack
STACKSIZE		= 8k
COMPRESS		= upx
OPTIMISATION	= -O2 -fomit-frame-pointer -finline-functions
CPU				= -m68020-60
INCLUDE_PATH	= /usr/GEM/include
LIB_PATH		= /usr/GEM/lib
LIB				= -lshare -lm2060 -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

Z_OBJ			= zlib/adler32.o zlib/crc32.o zlib/deflate.o zlib/inffast.o \
				  zlib/inflate.o zlib/infback.o  zlib/inftrees.o zlib/trees.o \
    	   		  zlib/uncompr.o zlib/zutil.o

TIFF_OBJ 		= libtiff/tif_aux.o libtiff/tif_close.o libtiff/tif_codec.o libtiff/tif_color.o \
				  libtiff/tif_compress.o libtiff/tif_dir.o libtiff/tif_dirinfo.o libtiff/tif_dirread.o \
				  libtiff/tif_dirwrite.o libtiff/tif_dumpmode.o libtiff/tif_error.o libtiff/tif_extension.o \
				  libtiff/tif_fax3.o libtiff/tif_fax3sm.o libtiff/tif_flush.o libtiff/tif_getimage.o libtiff/tif_jpeg.o \
				  libtiff/tif_luv.o libtiff/tif_lzw.o libtiff/tif_next.o libtiff/tif_open.o \
				  libtiff/tif_packbits.o libtiff/tif_pixarlog.o libtiff/tif_predict.o libtiff/tif_read.o \
				  libtiff/tif_strip.o libtiff/tif_swab.o libtiff/tif_thunder.o libtiff/tif_tile.o libtiff/tif_atari.o \
				  libtiff/tif_version.o libtiff/tif_warning.o libtiff/tif_write.o libtiff/tif_zip.o

JPG_OBJ			= libjpg/jcomapi.o libjpg/jutils.o libjpg/jerror.o libjpg/jmemmgr.o \
				  libjpg/jmemnobs.o libjpg/jdapimin.o libjpg/jdapistd.o libjpg/jdtrans.o libjpg/jdatasrc.o \
    	   		  libjpg/jdmaster.o libjpg/jdinput.o libjpg/jdmarker.o libjpg/jdhuff.o libjpg/jdphuff.o \
        		  libjpg/jdmainct.o libjpg/jdcoefct.o libjpg/jdpostct.o libjpg/jddctmgr.o \
        		  libjpg/jidctfst.o libjpg/jidctred.o libjpg/jdsample.o libjpg/jdcolor.o libjpg/jquant1.o libjpg/jquant2.o libjpg/jdmerge.o \
				  libjpg/jcapimin.o libjpg/jcapistd.o libjpg/jctrans.o libjpg/jcparam.o \
		          libjpg/jdatadst.o libjpg/jcinit.o libjpg/jcmaster.o libjpg/jcmarker.o libjpg/jcmainct.o \
		          libjpg/jcprepct.o libjpg/jccoefct.o libjpg/jccolor.o libjpg/jcsample.o libjpg/jchuff.o \
        		  libjpg/jcphuff.o libjpg/jcdctmgr.o libjpg/jfdctfst.o

OBJ				=  $(Z_OBJ) $(JPG_OBJ) $(TIFF_OBJ) tiffldg.o 

PROGRAM 		= tiff.ldg


all: $(PROGRAM) strip stack compress

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -Xlinker -L$(LIB_PATH) $(LIB)

strip:    
	$(STRIP) $(PROGRAM)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)

clean: 
	rm -f *.o libtiff/*.o zlib/*.o libjpg/*.o *.ldg	 
