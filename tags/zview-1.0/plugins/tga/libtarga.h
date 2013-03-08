#ifndef _libtarga_h_
#define _libtarga_h_

#define TGA_IMG_NODATA             (0)
#define TGA_IMG_UNC_PALETTED       (1)
#define TGA_IMG_UNC_TRUECOLOR      (2)
#define TGA_IMG_UNC_GRAYSCALE      (3)
#define TGA_IMG_RLE_PALETTED       (9)
#define TGA_IMG_RLE_TRUECOLOR      (10)
#define TGA_IMG_RLE_GRAYSCALE      (11)


#define TGA_LOWER_LEFT             (0)
#define TGA_LOWER_RIGHT            (1)
#define TGA_UPPER_LEFT             (2)
#define TGA_UPPER_RIGHT            (3)


#define HDR_LENGTH               (18)
#define HDR_IDLEN                (0)
#define HDR_CMAP_TYPE            (1)
#define HDR_IMAGE_TYPE           (2)
#define HDR_CMAP_FIRST           (3)
#define HDR_CMAP_LENGTH          (5)
#define HDR_CMAP_ENTRY_SIZE      (7)
#define HDR_IMG_SPEC_XORIGIN     (8)
#define HDR_IMG_SPEC_YORIGIN     (10)
#define HDR_IMG_SPEC_WIDTH       (12)
#define HDR_IMG_SPEC_HEIGHT      (14)
#define HDR_IMG_SPEC_PIX_DEPTH   (16)
#define HDR_IMG_SPEC_IMG_DESC    (17)

typedef struct
{
    uint8	idlen;               /* length of the image_id string below.		*/
    uint8	cmap_type;           /* paletted image <=> cmap_type				*/
    uint8	image_type;          /* can be any of the IMG_TYPE constants above.	*/
    uint16	cmap_first;          
    uint16	cmap_length;         /* how long the colormap is					*/
    uint8	cmap_entry_size;     /* how big a palette entry is.					*/
    uint16	img_spec_xorig;      /* the x origin of the image in the image data.*/
    uint16	img_spec_yorig;      /* the y origin of the image in the image data.*/
    uint16	img_spec_width;      /* the width of the image.						*/
    uint16	img_spec_height;     /* the height of the image.					*/
    uint8	img_spec_pix_depth;  /* the depth of a pixel in the image.			*/
    uint8	img_spec_img_desc;   /* the image descriptor.						*/
} targa_hdr;

#define TGA_TRUECOLOR_32 (4)
#define TGA_TRUECOLOR_24 (3)

typedef struct
{
	int16		handle;
	uint8 		*img_buf;			/* buffer for 1 line of packet TGA Data */
	uint8		alphabits;
	uint8		bytes_per_pix;
	uint8		orientation;
	int32		line_size;			
	int32		img_buf_len;		/* L„nge des IMG-Buffers */
	int32		img_buf_offset;		/* Abstand zum Anfang des IMG-Buffers */
	int32		img_buf_used;		/* Anzahl der benutzten Bytes des IMG-Buffers */
	int32		rest_length;		/* noch einzulesende Dateil„nge */
	targa_hdr	tga;
} tga_pic;

#endif /* _libtarga_h_ */
