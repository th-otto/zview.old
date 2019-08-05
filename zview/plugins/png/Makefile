top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg -lpng -lz -lm
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DPNGLIB_SLB -DZLIB_SLB -DSHAREDLIB=\"zvpng.slb\"

LIBPNG_OBJS = libpng/png.o libpng/pngerror.o \
	libpng/pngget.o libpng/pngmem.o libpng/pngpread.o libpng/pngread.o libpng/pngrio.o libpng/pngrtran.o libpng/pngrutil.o \
	libpng/pngset.o libpng/pngtrans.o libpng/pngwio.o libpng/pngwrite.o libpng/pngwtran.o libpng/pngwutil.o

LIBPNG_SOBJS =


all:: png.ldg zvpng.slb

include $(top_srcdir)/rules

png.ldg: pngldg.o png.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvpng.slb: header.lo pngslb.lo png.lo $(LIBPNG_SOBJS)
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lpng.slb -lz.slb -lm $(LDSHAREDLIBC)

header.lo pngslb.lo png.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb libpng/*.o libpng/*.lo
