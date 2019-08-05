top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvdegas.slb\"

all:: degas.ldg zvdegas.slb

include $(top_srcdir)/rules

degas.ldg: degasldg.o degas.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvdegas.slb: header.lo degasslb.lo degas.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)

header.lo degasslb.lo degas.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
