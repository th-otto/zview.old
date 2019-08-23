#  makefile for libshare/test.ldg:
#  by Arnaud BERCEGEAY (Feb 2004)

top_srcdir=../../../..
include $(top_srcdir)/configvars

LIBOBJ = libtest.o ../../../ldglib/ldglib.o ../../../ldglib/ldgutil.o
PRGOBJ = prgtest.o ../../../ldglib/ldgapp.c ../../../ldglib/ldgload.c ../../../ldglib/ldgutil.c

LIBTARGET = test.ldg 
PRGTARGET = test.tos

all:: $(LIBTARGET) $(PRGTARGET)

$(LIBTARGET): $(LIBOBJ)
	$(CC) -o $@ $(LIBOBJ) ../libshare.a -lgem

$(PRGTARGET): $(PRGOBJ)
	$(CC) -o $@ $(PRGOBJ) -lgem

clean:
	rm -f *.o $(LIBTARGET) $(PRGTARGET)
