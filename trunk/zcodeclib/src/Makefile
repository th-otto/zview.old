DEBUG                   = # -DDEBUG
STACKSIZE               = 64k
OPTIMISATION            = -O2 -fomit-frame-pointer
CPU                     = -m68020-60

CFLAGS                  = $(CPU) $(OPTIMISATION) $(DEBUG) -Wall -I../include -I../../common

VDI_OBJ			= zvdi/p2c.o zvdi/color.o zvdi/dither.o zvdi/raster.o zvdi/raresize.o zvdi/vdi.o
OBJ			= $(VDI_OBJ)  img.o resize.o codecs.o mfdb.o load_img.o ztext.o

PROGRAM			= ../lib/m68020-60/libzcodec.a

# override
default: $(PROGRAM)

include ../../common.mak

# override
$(PROGRAM): $(OBJ)
	rm -f $@
	$(AR) cru $@ $^
