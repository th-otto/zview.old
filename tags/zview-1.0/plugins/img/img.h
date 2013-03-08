/*------------------------------------------*/
/* 				IMG Structur 				*/
/*------------------------------------------*/

#ifndef	__IMG_IMAGE__
#define	__IMG_IMAGE__

#define XIMG  0x58494D47

typedef struct
{
	int16		version;
	int16		length;
	int16		planes;
	int16		pattern_length;
	int16		pix_width;
	int16		pix_height;
	int16		w;
	int16		h;
} IMGHDR;

#endif


typedef struct
{
	uint8 		*img_buf;	
	int32		img_buf_len;	
	int32		img_buf_offset;		
	int32		img_buf_used;	
	uint8		*line_buffer;	
	int16		pat_len;		
	int16		line_len;		
	int32		rest_length;	
	int32		file_length;
	int16		img_buf_valid;
} XIMG_DATA;

#ifndef	__PIC_STRUCT__
#define	__PIC_STRUCT__

typedef struct
{
	uint8 red, green, blue;
}RGB;

typedef struct _picture
{
	int16		file_handle;
	int16		width;	
	int16		height;	
	int16		planes;	
	IMGHDR		img;
	XIMG_DATA	info;
} IMG_REF;

#endif


