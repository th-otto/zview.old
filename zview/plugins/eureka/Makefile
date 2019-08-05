top_srcdir=../../..
include $(top_srcdir)/configvars

LIB       = ../libshare/libshare.a -lldg
STACKSIZE = 64k

CPPFLAGS		+= -I../../../common -I../common -I../..
SFLAGS += -DPLUGIN_SLB -DSHAREDLIB=\"zveurek.slb\"

all:: eureka.ldg zveurek.slb

include $(top_srcdir)/rules

eureka.ldg: eurekaldg.o eureka.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)

zveurek.slb: header.lo eurekaslb.lo eureka.lo
	$(LDSHARED) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDSHAREDLIBC)

header.lo eurekaslb.lo eureka.lo: ../common/plugin.h ../common/zview.h

clean:
	rm -f *.o *.lo *.a *.ldg *.slb
