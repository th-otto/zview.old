top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg -ltiff -ljpeg -llzma -lz
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DTIFF_SLB -DJPEG_SLB -DLZMA_SLB -DZLIB_SLB -DSHAREDLIB=\"zvtiff.slb\"

all:: tiff.ldg zvtiff.slb

include $(top_srcdir)/rules

tiff.ldg: tiffldg.o tiff.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvtiff.slb: header.lo tiffslb.lo tiff.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ltiff.slb -ljpeg.slb -llzma.slb -lz.slb $(LDSHAREDLIBC)

header.lo tiffslb.lo tiff.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
