top_srcdir=../../..
include $(top_srcdir)/configvars

LDGOBJS = ../../ldglib/ldglib.o ../../ldglib/ldgcook.o ../../ldglib/ldgutil.o
LIB       = ../libshare/libshare.a -ltiff -ljpeg -llzma -lz
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DTIFF_SLB -DJPEG_SLB -DLZMA_SLB -DZLIB_SLB -DSHAREDLIB=\"zvtiff.slb\"

all:: tiff.ldg zvtiff.slb

include $(top_srcdir)/rules

tiff.ldg: tiffldg.o tiff.o $(LDGOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB) -lm

zvtiff.slb: header.lo tiffslb.lo tiff.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ltiff.slb -ljpeg.slb -llzma.slb -lz.slb $(LDSHAREDLIBC) -lm
	$(STRIPEX) $@

header.lo tiffslb.lo tiff.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
