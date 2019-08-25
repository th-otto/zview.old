top_srcdir=../..
include $(top_srcdir)/configvars

CFLAGS                  += -I../include -I../../common

VDI_OBJ			= zvdi/p2c.o zvdi/color.o zvdi/dither.o zvdi/raster.o zvdi/raresize.o zvdi/vdi.o
OBJ			= $(VDI_OBJ)  img.o resize.o codecs.o mfdb.o load_img.o ztext.o

PROGRAM			= ../lib/$(CPU_SUBDIR)libzcodec.a

# override
all:: $(PROGRAM)

# override
$(PROGRAM): $(OBJ)
	mkdir -p ../lib/$(CPU_SUBDIR)
	rm -f $@
	$(AR) cru $@ $^


clean::
	rm -f $(OBJ) $(PROGRAM)
