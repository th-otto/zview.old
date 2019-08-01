top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvbmp.slb\"

all:: bmp.ldg zvbmp.slb

include $(top_srcdir)/rules

bmp.ldg: bmpldg.o bmp.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvbmp.slb: header.lo bmpslb.lo bmp.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)

header.lo bmpslb.lo bmp.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
