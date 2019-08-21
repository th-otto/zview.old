top_srcdir=../../..
include $(top_srcdir)/configvars

LDGOBJS = ../../ldglib/ldglib.o ../../ldglib/ldgcook.o ../../ldglib/ldgutil.o
GIFLIB = -lgif
LIB       = ../libshare/libshare.a $(GIFLIB)
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvgif.slb\"

all:: gif.ldg zvgif.slb

include $(top_srcdir)/rules

gif.ldg: gifldg.o gif.o $(LDGOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvgif.slb: header.lo gifslb.lo gif.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(GIFLIB) $(LDFLAGS) $(LDSHAREDLIBC)

header.lo gifslb.lo gif.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
