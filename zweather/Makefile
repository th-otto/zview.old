USE_WINDOM2=1
top_srdir=..
include $(top_srcdir)/configvars

LIBXML2_INC		= /usr/m68k-atari-mint/sys-root/usr/include/libxml2

ZCODECLIB               = ../zcodeclib/lib/$(CPU_SUBDIR)libzcodec.a

STACKSIZE		= 64k
LIB			= $(WINDOM_LIBS) $(ZCODECLIB) -lldg -lgem -lxml2 -liconv -lm -lz
CFLAGS			+= -I$(LIBXML2_INC) -I../common -I../zcodeclib/include
OBJ 			= inet.o http_client.o parsers.o get_data.o \
			  debug.o zstring.o dialog.o prefs.o zaes.o icon.o main.o

PROGRAM 		= zweather.app

all:: $(PROGRAM)

$(PROGRAM): $(OBJ) $(ZCODECLIB)
	$(CXX) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)
	: cp $(PROGRAM) _dist/$(PROGRAM)
	$(COMPRESS) $(PROGRAM)


clean::
	rm -f $(OBJ) $(PROGRAM)
