ifeq ($(CROSS),yes)
AS		= m68k-atari-mint-as
AR		= m68k-atari-mint-ar 
CC		= m68k-atari-mint-gcc
STACK		= m68k-atari-mint-stack
else
AS		= as
CC		= gcc
STACK		= stack
endif

OPTIMISATION	= -O2 -fomit-frame-pointer
CPU		= -m68020-60

CFLAGS		= $(CPU) $(OPTIMISATION) -Wall -I../include -I/usr/local/include

VDI_OBJ 	= zvdi/p2c.o zvdi/color.o zvdi/dither.o zvdi/raster.o zvdi/raresize.o zvdi/vdi.o

OBJ 		= $(VDI_OBJ)  img.o resize.o codecs.o mfdb.o load_img.o ztext.o mem.o

TARGET		= ../lib/libzcodec.a

$(TARGET): $(OBJ)
	rm -f $@
	$(AR) cru $@ $^

clean:
	rm -f *.o zvdi/*.o *.O zvdi/*.O *~ $(TARGET)
