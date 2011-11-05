#define UP_TO_DOWN 		0
#define DOWN_TO_UP		1
#define ST_RAM			0
#define TT_RAM			1

typedef enum 
{
	JCS_UNKNOWN,		/* error/unspecified */
	JCS_GRAYSCALE,		/* monochrome */
	JCS_RGB,			/* red/green/blue */
	JCS_YCbCr,			/* Y/Cb/Cr (also known as YUV) */
	JCS_CMYK,			/* C/M/Y/K */
	JCS_YCCK			/* Y/Cb/Cr/K */
} J_COLOR_SPACE;

typedef struct
{
	uint8	red;
	uint8	green;
	uint8	blue;
} COLOR_MAP;


typedef struct _img_info *IMGINFO;

typedef struct _img_info
{
	/* Data returned by the decoder Picture's initialisation */
	uint16    	width;					/* Picture width 											*/
	uint16		height;					/* Picture Height											*/
	uint16 		components;  			/* Picture components 										*/
	uint16	 	planes;					/* Picture bits by pixel									*/
	uint32     	colors;					/* Picture colors number									*/
	int16		orientation;			/* picture orientation: UP_TO_DOWN or DONW_TO_UP ( ex: TGA)	*/
	uint16		page;					/* Number of page/image in the file 						*/
	uint16		delay;					/* Animation delay in 200th sec. between each frame.		*/
	uint16		num_comments;			/* Lines in the file's comment. ( 0 if none) 	 			*/
	uint16		max_comments_length;	/* The length of the greater line in the comment 			*/
	int16		indexed_color;			/* Picture has a palette: 0) No 1) Yes						*/
	int16		memory_alloc;			/* where to do the memory allocation for the reader_read 
										   function buffer, possible values are ST_RAM or TT_RAM	*/
	COLOR_MAP	palette[256];			/* Palette in standard RGB format							*/


	/* data send to the codec */
	uint32		background_color;		/* The wanted background's color in hexa */		
	int16		thumbnail;				/* 1 if true else 0.
										   Some format have thumbnail inside the picture, if 1, we 
										   want only this thumbnail and not the image itself 		*/
	uint16		page_wanted;			/* The wanted page to get for the reader_read() function 											   
										   The first page is 0 and the last is img_info->page - 1	*/											   


	/* The following variable are only for Information Prupose, the zview doesn't 
	   use it for decoding routine but only for the information dialog								*/ 
	int8		info[40];				/* Picture's info, for ex: "Gif 87a" 						*/
	int8		compression[5];			/* Compression type, ex: "LZW" 								*/
	uint16    	real_width;				/* Real picture width( if the thumbnail variable is 1)		*/
	uint16		real_height;			/* Ral picture Height( if the thumbnail variable is 1)		*/


	/* Private data for the plugin */
	void		*_priv_ptr;			
	void		*_priv_ptr_more;			
	void		*__priv_ptr_more;

	int32		_priv_var;
	int32		_priv_var_more;

} img_info;


typedef struct _dec_data *DECDATA;

typedef struct _dec_data
{
	uint8  		*RowBuf;
	void   		*DthBuf;
	uint8		*DstBuf;
	uint16    	DthWidth;
	uint16    	PixMask;
	int32   	LnSize;
	uint32   	IncXfx;
	uint32   	IncYfx;
	uint32    	Pixel[256];
} dec_data;

extern void ( *raster)	  	( DECDATA, void *dst);
extern void ( *raster_cmap) ( DECDATA, void *);
extern void ( *raster_true) ( DECDATA, void *);
extern void ( *rasterize_32) ( DECDATA, void *);
extern void ( *cnvpal_color)( IMGINFO, DECDATA);
extern void ( *raster_gray) ( DECDATA, void *);
extern boolean get_pic_info( const char *file, char *extention, IMGINFO info);
extern void quit_img( IMGINFO info, DECDATA data);
extern boolean decoder_init_done;
