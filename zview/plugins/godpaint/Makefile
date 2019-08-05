top_srcdir=../../..
include $(top_srcdir)/configvars

LIB			= ../libshare/libshare.a -lldg
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvgod.slb\"

all:: godpaint.ldg zvgod.slb

include $(top_srcdir)/rules

godpaint.ldg: godldg.o god.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvgod.slb: header.lo godslb.lo god.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)

header.lo godslb.lo god.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
