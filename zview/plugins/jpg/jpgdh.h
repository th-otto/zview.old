/*	JPEGD header file
	(C) 1992-93 Brainstorm & Atari Corporation.
	to be included after vdi.h
	Last modification on 8-apr-1993.
*/

#if defined(__atarist__) || defined(__TOS__)
#define VOID_PTR	void *
#define UCHAR_PTR	unsigned char *
#define FUNC_PTR(x,y)	short (*x)(y)
#else
/* otherwise in emulator */
#define	VOID_PTR	uint32_t
#define	UCHAR_PTR	uint32_t
#define FUNC_PTR(x,y)	uint32_t x
#endif

typedef struct _JPGD_STRUCT JPGD_STRUCT;
typedef JPGD_STRUCT	*JPGD_PTR;

struct _JPGD_STRUCT {
	VOID_PTR	InPointer;							/* JPEG Image Pointer */
	VOID_PTR	OutPointer;							/* Output Buffer/Filename Pointer (see OutFlag) */
	int32_t	InSize;									/* JPEG Image Size (Bytes) */
	int32_t	OutSize;								/* Output Image Size (Bytes) */
	short	InComponents;							/* JPEG Image Components Number (1->4) */
	short	OutComponents;							/* Output Components Number (1->4) */
	short	OutPixelSize;							/* Output Pixel Size (1->4) */
	short	OutFlag;								/* 0 (RAM Output) / -1 (Disk Output) */
	short	XLoopCounter;							/* Number of MCUs per Row */
	short	YLoopCounter;							/* Number of MCUs per Column */
	FUNC_PTR(Create, JPGD_STRUCT *);				/* Pointer to User Routine / NULL */
	FUNC_PTR(Write, JPGD_STRUCT *);					/* Pointer to User Routine / NULL */
	FUNC_PTR(Close, JPGD_STRUCT *);					/* Pointer to User Routine / NULL */
	FUNC_PTR(SigTerm, JPGD_STRUCT *);				/* Pointer to User Routine / NULL */
	UCHAR_PTR	Comp1GammaPtr;						/* Component 1 Gamma Table / NULL */
	UCHAR_PTR	Comp2GammaPtr;						/* Component 2 Gamma Table / NULL */
	UCHAR_PTR	Comp3GammaPtr;						/* Component 3 Gamma Table / NULL */
	UCHAR_PTR	Comp4GammaPtr;						/* Component 4 Gamma Table / NULL */
	FUNC_PTR(UserRoutine, JPGD_STRUCT *);			/* Pointer to User Routine (Called during Decompression) / NULL */
	VOID_PTR	OutTmpPointer;						/* Current OutPointer / Temporary Disk Buffer Pointer (see OutFlag) */
	short	MCUsCounter;							/* Number of MCUs not Decoded */
	short	OutTmpHeight;							/* Number of Lines in OutTmpPointer */
	int32_t	User[2];								/* Free, Available for User */
	short	OutHandle;								/* 0 / Output File Handle (see OutFlag) */
	MFDB	MFDBStruct;								/* Output Image MFDB */

	/* Official structure stop here, what follows is decoder-dependant */
};

#define	JPGD_MAGIC	0x5F4A5044L /* '_JPD' */
#define	JPGD_VERSION	1

#undef NOERROR
enum _JPGD_ENUM {
	NOERROR=0,			/* File correctly uncompressed */
	UNKNOWNFORMAT,		/* File is not JFIF (Error) */
	INVALIDMARKER,		/* Reserved CCITT Marker Found (Error) */
	SOF1MARKER,			/* Mode not handled by the decoder (Error) */
	SOF2MARKER,			/* Mode not handled by the decoder (Error) */
	SOF3MARKER,			/* Mode not handled by the decoder (Error) */
	SOF5MARKER,			/* Mode not handled by the decoder (Error) */
	SOF6MARKER,			/* Mode not handled by the decoder (Error) */
	SOF7MARKER,			/* Mode not handled by the decoder (Error) */
	SOF9MARKER,			/* Mode not handled by the decoder (Error) */
	SOF10MARKER,		/* Mode not handled by the decoder (Error) */
	SOF11MARKER,		/* Mode not handled by the decoder (Error) */
	SOF13MARKER,		/* Mode not handled by the decoder (Error) */
	SOF14MARKER,		/* Mode not handled by the decoder (Error) */
	SOF15MARKER,		/* Mode not handled by the decoder (Error) */
	RSTmMARKER,			/* Unexpected RSTm found (Error) */
	BADDHTMARKER,		/* Buggy DHT Marker (Error) */
	DACMARKER,			/* Marker not handled by the decoder (Error) */
	BADDQTMARKER,		/* Buggy DQT Marker (Error) */
	BADDNLMARKER,		/* Invalid/Unexpected DNL Marker (Error) */
	BADDRIMARKER,		/* Invalid DRI Header Size (Error) */
	DHPMARKER,			/* Marker not handled by the decoder (Error) */
	EXPMARKER,			/* Marker not handled by the decoder (Error) */
	BADSUBSAMPLING,		/* Invalid components subsampling (Error) */
	NOTENOUGHMEMORY,	/* Not enough memory... (Error) */
	DECODERBUSY,		/* Decoder is busy (Error) */
	DSPBUSY,			/* Can't lock the DSP (Error) */
	BADSOFnMARKER,		/* Buggy SOFn marker (Error) */
	BADSOSMARKER,		/* Buggy SOS marker (Error) */
	HUFFMANERROR,		/* Buggy Huffman Stream (Error) */
	BADPIXELFORMAT,		/* Invalid Output Pixel Format (Error) */
	DISKFULL,			/* Hard/Floppy Disk Full (Error) */
	MISSINGMARKER,		/* Marker expected but not found (Error) */
	IMAGETRUNCATED,		/* More bytes Needed (Error) */
	EXTRABYTES,			/* Dummy Bytes after EOI Marker (Warning) */
	USERABORT,			/* User Routine signaled 'Abort' */
	DSPMEMORYERROR,		/* Not Enough DSP RAM (Error) */
	NORSTmMARKER,		/* RSTm Marker expected but not found */
	BADRSTmMARKER,		/* Invalid RSTm Marker Number */
	DRIVERCLOSED,		/* Driver is Already Closed. */
	ENDOFIMAGE			/* Stop Decoding (Internal Message, Should Never Appear) */
};
typedef	long	JPGD_ENUM;

/*
 * Note: all this functions use Pure-C calling convention:
 * - jpeg structure pointer passed in a0
 * - return value in d0
 * - d1-d2/a0-a1 are clobbered
 */
typedef struct {
	long		JPGDVersion;
	JPGD_ENUM	(*JPGDOpenDriver)(JPGD_PTR);
	JPGD_ENUM	(*JPGDCloseDriver)(JPGD_PTR);
	long		(*JPGDGetStructSize)(void);
	JPGD_ENUM	(*JPGDGetImageInfo)(JPGD_PTR);
	JPGD_ENUM	(*JPGDGetImageSize)(JPGD_PTR);
	JPGD_ENUM	(*JPGDDecodeImage)(JPGD_PTR);
} JPGDDRV_STRUCT;
typedef JPGDDRV_STRUCT	*JPGDDRV_PTR;
