include ../../../configvars

LIB			= ../libshare/libshare.a -lldg -ljpeg -lexif -lm

CFLAGS			+= -I../../../common -I../common

DSP_OBJ			= jpgdsp.o 

OBJ			= $(DSP_OBJ) jpgldg.o 

PROGRAM 		= jpg.ldg

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,64k $(LIB)

clean:
	rm -f *.o *.a *.ldg
