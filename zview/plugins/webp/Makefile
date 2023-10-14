top_srcdir=../../..
include $(top_srcdir)/configvars

LDGOBJS = ../../ldglib/ldglib.o ../../ldglib/ldgcook.o ../../ldglib/ldgutil.o
LIB       = -lwebpdemux -lwebp -lz -lm
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zvwebp.slb\"

# libwebp headers don't have any version info, extract it from pkg-config
export PKG_CONFIG_PATH := $(shell CROSS_PREFIX=$(CROSS_PREFIX); CROSS_PREFIX=$${CROSS_PREFIX%-}; echo /usr/$${CROSS_PREFIX}/lib/pkgconfig)
WEBP_VERSION_STR := $(shell pkg-config --modversion libwebp)
CPPFLAGS += -DWEBP_VERSION_STR=\"$(WEBP_VERSION_STR)\"

all:: webp.ldg zvwebp.slb

include $(top_srcdir)/rules

webp.ldg: webpldg.o webp.o $(LDGOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zvwebp.slb: header.lo webpslb.lo webp.lo $(webp_SOBJS)
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lwebpdemux -lwebp $(LDSHAREDLIBC)
	$(STRIPEX) $@

header.lo webpslb.lo webp.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
