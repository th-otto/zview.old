top_srcdir=../../..
include $(top_srcdir)/configvars

LIB			= ../libshare/libshare.a -lldg -ljpeg -lexif -lm
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common
CPPFLAGS += -DNO_GETENV -DJPEG_LIB_VERSION=80
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvjpg.slb\"

libexif_OBJS = \
	libexif/exif-byte-order.o	\
	libexif/exif-content.o		\
	libexif/exif-data.o		\
	libexif/exif-entry.o		\
	libexif/exif-format.o		\
	libexif/exif-ifd.o		\
	libexif/exif-loader.o		\
	libexif/exif-log.o		\
	libexif/exif-mem.o		\
	libexif/exif-mnote-data.o	\
	libexif/exif-tag.o		\
	libexif/exif-utils.o \
	libexif/canon/exif-mnote-data-canon.o \
	libexif/canon/mnote-canon-entry.o \
	libexif/canon/mnote-canon-tag.o \
	libexif/fuji/mnote-fuji-entry.o \
	libexif/fuji/exif-mnote-data-fuji.o \
	libexif/fuji/mnote-fuji-tag.o \
	libexif/olympus/mnote-olympus-entry.o \
	libexif/olympus/exif-mnote-data-olympus.o \
	libexif/olympus/mnote-olympus-tag.o \
	libexif/pentax/mnote-pentax-entry.o \
	libexif/pentax/exif-mnote-data-pentax.o \
	libexif/pentax/mnote-pentax-tag.o \
	$(empty)

libexif_SOBJS = $(libexif_OBJS:.o=.lo)

JPEG = jpeg-8d
jpeg_OBJS = \
	$(JPEG)/jcapimin.o $(JPEG)/jcapistd.o $(JPEG)/jccoefct.o $(JPEG)/jccolor.o $(JPEG)/jcdctmgr.o $(JPEG)/jchuff.o \
	$(JPEG)/jcinit.o $(JPEG)/jcmainct.o $(JPEG)/jcmarker.o $(JPEG)/jcmaster.o $(JPEG)/jcomapi.o $(JPEG)/jcparam.o \
	$(JPEG)/jcprepct.o $(JPEG)/jcsample.o $(JPEG)/jctrans.o $(JPEG)/jdapimin.o $(JPEG)/jdapistd.o \
	$(JPEG)/jdatadst.o $(JPEG)/jdatasrc.o $(JPEG)/jdcoefct.o $(JPEG)/jdcolor.o $(JPEG)/jddctmgr.o $(JPEG)/jdhuff.o \
	$(JPEG)/jdinput.o $(JPEG)/jdmainct.o $(JPEG)/jdmarker.o $(JPEG)/jdmaster.o $(JPEG)/jdmerge.o \
	$(JPEG)/jdpostct.o $(JPEG)/jdsample.o $(JPEG)/jdtrans.o $(JPEG)/jerror.o $(JPEG)/jfdctflt.o $(JPEG)/jfdctfst.o \
	$(JPEG)/jfdctint.o $(JPEG)/jidctflt.o $(JPEG)/jidctfst.o $(JPEG)/jidctint.o $(JPEG)/jquant1.o \
	$(JPEG)/jquant2.o $(JPEG)/jutils.o $(JPEG)/jmemmgr.o $(JPEG)/jmemnobs.o $(JPEG)/jcicc.o $(JPEG)/jdicc.o \
	$(JPEG)/jaricom.o $(JPEG)/jcarith.o $(JPEG)/jdarith.o \
	$(empty)

jpeg_SOBJS = $(jpeg_OBJS:.o=.lo)

all:: jpg.ldg zvjpg.slb

include $(top_srcdir)/rules

jpg.ldg: jpgldg.o jpg.o jpgdsp.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvjpg.slb: header.lo jpgslb.lo jpg.lo jpgdsp.lo $(jpeg_SOBJS) $(libexif_SOBJS)
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lm $(LDSHAREDLIBC)

header.lo jpgslb.lo jpg.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb libexif/*.o libexif/*.lo libexif/*/*.o libexif/*/*.lo jpeg-*/*.o jpeg-*/*.lo
