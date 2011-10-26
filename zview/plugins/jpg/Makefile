STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lshare -lldg -lexif -lm

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common

JPG_OBJ			= libjpg/jcomapi.o libjpg/jutils.o libjpg/jerror.o libjpg/jmemmgr.o \
			  libjpg/jmemnobs.o libjpg/jdapimin.o libjpg/jdapistd.o libjpg/jdtrans.o libjpg/jdatasrc.o \
    	   		  libjpg/jdmaster.o libjpg/jdinput.o libjpg/jdmarker.o libjpg/jdhuff.o libjpg/jdphuff.o \
        		  libjpg/jdmainct.o libjpg/jdcoefct.o libjpg/jdpostct.o libjpg/jddctmgr.o \
        		  libjpg/jidctfst.o libjpg/jidctred.o libjpg/jdsample.o libjpg/jdcolor.o libjpg/jquant1.o libjpg/jquant2.o libjpg/jdmerge.o \
			  libjpg/jcapimin.o libjpg/jcapistd.o libjpg/jctrans.o libjpg/jcparam.o \
		          libjpg/jdatadst.o libjpg/jcinit.o libjpg/jcmaster.o libjpg/jcmarker.o libjpg/jcmainct.o \
		          libjpg/jcprepct.o libjpg/jccoefct.o libjpg/jccolor.o libjpg/jcsample.o libjpg/jchuff.o \
        		  libjpg/jcphuff.o libjpg/jcdctmgr.o libjpg/jfdctfst.o

DSP_OBJ			= jpgdsp.o 

OBJ			= $(JPG_OBJ) $(DSP_OBJ) jpgldg.o 

PROGRAM 		= jpg.ldg

include ../../../common.mak
