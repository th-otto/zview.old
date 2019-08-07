top_srcdir=../../..
include $(top_srcdir)/configvars

LIB			= ../libshare/libshare.a -lldg -ljpeg -lexif -lm
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common
CPPFLAGS += -DNO_GETENV -DJPEG_LIB_VERSION=80
SFLAGS += -DPLUGIN_SLB -DJPEG_SLB -DEXIF_SLB -DSHAREDLIB=\"zvjpg.slb\"

libexif_OBJS =

libexif_SOBJS = $(libexif_OBJS:.o=.lo)

all:: jpg.ldg zvjpg.slb

include $(top_srcdir)/rules

jpg.ldg: jpgldg.o jpg.o jpgdsp.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvjpg.slb: header.lo jpgslb.lo jpg.lo jpgdsp.lo $(jpeg_SOBJS) $(libexif_SOBJS)
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ljpeg8d.slb -lexif.slb -lm $(LDSHAREDLIBC)

header.lo jpgslb.lo jpg.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb libexif/*.o libexif/*.lo libexif/*/*.o libexif/*/*.lo jpeg-*/*.o jpeg-*/*.lo
