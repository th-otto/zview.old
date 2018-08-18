include ../../../configvars

LIB			= ../libshare/libshare.a -ltiff -ljpeg -lldg -lz -llzma -lm

CFLAGS			+= -I../../../common -I../common

OBJ			= tiffldg.o

PROGRAM 		= tiff.ldg

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,64k $(LIB)

clean:
	rm -f *.o *.a *.ldg
