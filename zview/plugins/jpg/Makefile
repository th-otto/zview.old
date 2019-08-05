top_srcdir=../../..
include $(top_srcdir)/configvars

LIB			= ../libshare/libshare.a -lldg -ljpeg -lexif -lm
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common
CPPFLAGS += -DNO_GETENV -DJPEG_LIB_VERSION=80
SFLAGS += -DPLUGIN_SLB -DJPEG_SLB -DSHAREDLIB=\"zvjpg.slb\"

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

all:: jpg.ldg zvjpg.slb

include $(top_srcdir)/rules

jpg.ldg: jpgldg.o jpg.o jpgdsp.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvjpg.slb: header.lo jpgslb.lo jpg.lo jpgdsp.lo $(jpeg_SOBJS) $(libexif_SOBJS)
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ljpeg8d.slb -lm $(LDSHAREDLIBC)

header.lo jpgslb.lo jpg.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb libexif/*.o libexif/*.lo libexif/*/*.o libexif/*/*.lo jpeg-*/*.o jpeg-*/*.lo
