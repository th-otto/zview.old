/*------------------------------------------*/
/* 				IMG Structur 				*/
/*------------------------------------------*/

#ifndef	__IMG_IMAGE__
#define	__IMG_IMAGE__

#define XIMG  0x58494D47

typedef struct
{
	int16_t		version;
	int16_t		length;
	int16_t		planes;
	int16_t		pattern_length;
	int16_t		pix_width;
	int16_t		pix_height;
	int16_t		width;
	int16_t		height;
} IMGHDR;

#endif


typedef struct
{
	uint8_t 	*img_buf;	
	int32_t		img_buf_len;	
	int32_t		img_buf_offset;		
	int32_t		img_buf_used;	
	uint8_t		*line_buffer;	
	int16_t		pat_len;		
	int16_t		line_len;		
	int32_t		rest_length;	
	int32_t		file_length;
	int16_t		img_buf_valid;
} XIMG_DATA;

#ifndef	__PIC_STRUCT__
#define	__PIC_STRUCT__

typedef struct
{
	uint8_t red, green, blue;
} RGB;

typedef struct _picture
{
	int16_t		file_handle;
	int16_t		width;	
	int16_t		height;	
	int16_t		planes;	
	IMGHDR		img;
	XIMG_DATA	info;
} IMG_REF;

#endif


