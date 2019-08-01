#include "plugins/common/imginfo.h"

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
extern boolean get_codec( const char *file);
extern boolean get_pic_info( const char *file, IMGINFO info);
extern void quit_img( IMGINFO info, DECDATA data);
extern boolean decoder_init_done;
