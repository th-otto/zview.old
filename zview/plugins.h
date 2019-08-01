#include <mint/slb.h>

extern int16 	plugins_init( void);
extern void 	plugins_quit( void);

struct _img_info;

struct _ldg_funcs {
	boolean	__CDECL ( *decoder_init)	(const char *, struct _img_info *);
	void 	__CDECL ( *decoder_quit)	(struct _img_info *);
	boolean	__CDECL ( *decoder_read)	(struct _img_info *, uint8 *);
	void	__CDECL ( *decoder_get_txt)	(struct _img_info *, txt_data *);
	void 	__CDECL ( *encoder_quit)	(struct _img_info *);
	boolean	__CDECL ( *encoder_write)	(struct _img_info *, uint8 *);
	boolean	__CDECL ( *encoder_init)	(const char *, struct _img_info *);
	void 	__CDECL ( *set_jpg_option)  (int16 set_quality, int /* J_COLOR_SPACE */ set_color_space, boolean set_progressive);
	void 	__CDECL ( *set_tiff_option) (int16 set_quality, uint16 set_encode_compression);
};

extern struct _ldg_funcs ldg_funcs;
extern SLB *curr_input_plugin;
extern SLB *curr_output_plugin;

typedef struct {
	int type;
#define CODEC_NONE 0
#define CODEC_LDG  1
#define CODEC_SLB  2
	union {
		LDG *ldg;
		SLB slb;
	} c;
	const char *extensions;
	long num_extensions;
} CODEC;
#define MAX_CODECS 100

extern CODEC codecs[MAX_CODECS];
extern int16 plugins_nbr;
