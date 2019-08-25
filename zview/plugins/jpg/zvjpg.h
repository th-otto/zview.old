#define M_EXIF 0xE1

#define GLOBAL_ERROR		0
#define DSP_ERROR			1
#define PROGRESSIVE_JPG		2
#define ALL_OK				3

void jpg_init(void);

/* Options */
extern int quality; /* quality 0 -> 100 % */
extern J_COLOR_SPACE color_space;
extern boolean progressive;
