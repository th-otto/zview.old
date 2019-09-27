top_srcdir=../../..
include $(top_srcdir)/configvars

LDGOBJS = ../../ldglib/ldglib.o ../../ldglib/ldgcook.o ../../ldglib/ldgutil.o
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvgod.slb\"

all:: godpaint.ldg zvgod.slb

include $(top_srcdir)/rules

godpaint.ldg: godldg.o god.o $(LDGOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvgod.slb: header.lo godslb.lo god.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)
	$(STRIPEX) $@

header.lo godslb.lo god.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
