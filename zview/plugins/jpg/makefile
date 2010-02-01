CC				= m68k-atari-mint-gcc
STACK			= m68k-atari-mint-stack
STACKSIZE		= 64k
OPTIMISATION	= -O2 -fomit-frame-pointer
CPU				= -m68020-60 -m68881
INCLUDE_PATH	= /home/zorro/cross-tools/include
LIB_PATH		= /home/zorro/cross-tools/lib
LIB				= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

JPG_OBJ			= libjpg/jcomapi.o libjpg/jutils.o libjpg/jerror.o libjpg/jmemmgr.o \
				  libjpg/jmemnobs.o libjpg/jdapimin.o libjpg/jdapistd.o libjpg/jdtrans.o libjpg/jdatasrc.o \
    	   		  libjpg/jdmaster.o libjpg/jdinput.o libjpg/jdmarker.o libjpg/jdhuff.o libjpg/jdphuff.o \
        		  libjpg/jdmainct.o libjpg/jdcoefct.o libjpg/jdpostct.o libjpg/jddctmgr.o \
        		  libjpg/jidctfst.o libjpg/jidctred.o libjpg/jdsample.o libjpg/jdcolor.o libjpg/jquant1.o libjpg/jquant2.o libjpg/jdmerge.o \
				  libjpg/jcapimin.o libjpg/jcapistd.o libjpg/jctrans.o libjpg/jcparam.o \
		          libjpg/jdatadst.o libjpg/jcinit.o libjpg/jcmaster.o libjpg/jcmarker.o libjpg/jcmainct.o \
		          libjpg/jcprepct.o libjpg/jccoefct.o libjpg/jccolor.o libjpg/jcsample.o libjpg/jchuff.o \
        		  libjpg/jcphuff.o libjpg/jcdctmgr.o libjpg/jfdctfst.o

EXIF_OBJ		= libexif/exif-byte-order.o libexif/exif-content.o libexif/exif-data.o libexif/exif-entry.o \
				  libexif/exif-format.o libexif/exif-ifd.o libexif/exif-loader.o libexif/exif-mem.o libexif/exif-mnote-data.o \
				  libexif/exif-tag.o libexif/exif-utils.o libexif/jpeg-marker.o \
				  libexif/canon/exif-mnote-data-canon.o libexif/canon/mnote-canon-entry.o libexif/canon/mnote-canon-tag.o \
				  libexif/olympus/exif-mnote-data-olympus.o libexif/olympus/mnote-olympus-entry.o libexif/olympus/mnote-olympus-tag.o \
				  libexif/pentax/exif-mnote-data-pentax.o libexif/pentax/mnote-pentax-entry.o libexif/pentax/mnote-pentax-tag.o

DSP_OBJ			= jpgdsp.o 

OBJ				= $(JPG_OBJ) $(EXIF_OBJ) $(DSP_OBJ) jpgldg.o 

PROGRAM 		= jpg.ldg


all: $(PROGRAM) stack

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -Xlinker -s -L$(LIB_PATH) $(LIB)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

clean: 
	rm -f *.o libjpg/*.o libexif/*.o *.ldg	 
