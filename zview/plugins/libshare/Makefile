#  makefile for libshare:
#  libshare is a dirty patch to make LDG compiled against mintlib shareable
#  by Arnaud BERCEGEAY (Feb 2004)

include ../../../configvars

OBJ = libshare.o gl_shm.o calloc.o chdir.o malloc.o realloc.o sbrk.o

TARGET = libshare.a
HEADERS = libshare.h lib.h

all: $(TARGET)

$(TARGET): $(OBJ)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJ): $(HEADERS)

install:
	cp -vf $(TARGET) $(DESTDIR)$(prefix)/lib
	cp -vf libshare.h $(DESTDIR)$(prefix)/include

clean:
	rm -f *.o *.a
	$(MAKE) -C test $@
