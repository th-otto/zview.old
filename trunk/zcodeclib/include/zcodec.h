#ifndef __ZCODECLIB_ZCODEC_H__
#define __ZCODECLIB_ZCODEC_H__

#include <types2b.h>

struct LDG;
struct MFDB;

typedef struct
{
	int16		page;		/* Number of page/image in the file 				*/
	uint16		*delay;		/* Animation delay in millise. between each frame	*/
	MFDB 		*image;		/* The Image itself									*/
}IMAGE;

extern int16	planar, vdi_handle, nplanes, vdi_work_in[10], vdi_work_out[57];
extern LDG		*ldg_mem;

extern int16	codecs_init( char *codec_name);
extern void		codecs_quit( void);
extern IMAGE	*load_img( const char *file, int16 w, int16 h, int16 keep_ratio);
extern void		delete_img( IMAGE *img);
extern MFDB 	*pic_resize( MFDB *in, int16 w, int16 h, int16 keep_ratio);
extern void		delete_mfdb( MFDB *bm, int16 nbr);

#endif
