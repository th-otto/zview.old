top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvneo.slb\"

all:: neo.ldg zvneo.slb

include $(top_srcdir)/rules

neo.ldg: neoldg.o neo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvneo.slb: header.lo neoslb.lo neo.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)

header.lo neoslb.lo neo.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
