top_srcdir=../../..
include $(top_srcdir)/configvars

LDGOBJS = ../../ldglib/ldglib.o ../../ldglib/ldgcook.o ../../ldglib/ldgutil.o
LIB       = ../libshare/libshare.a
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvimg.slb\"

all:: img.ldg zvimg.slb

include $(top_srcdir)/rules

img.ldg: imgldg.o img.o xp32.o $(LDGOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvimg.slb: header.lo imgslb.lo img.lo xp32.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)
	stripex $@

header.lo imgslb.lo img.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
