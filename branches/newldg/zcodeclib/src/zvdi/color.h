extern void 	save_colors( void);
extern int16 	remap_color( int32 value);

extern uint32 	color_lookup( uint32 rgb, int16 *trans);

extern void 	cnvpal_mono( IMGINFO info, DECDATA data);
extern void 	cnvpal_1_2( IMGINFO info, DECDATA data);
extern void 	cnvpal_4_8( IMGINFO info, DECDATA data);
extern void 	cnvpal_15( IMGINFO info, DECDATA data);
extern void 	cnvpal_high( IMGINFO info, DECDATA data);
extern void 	cnvpal_true( IMGINFO info, DECDATA data);

extern void 	( *cnvpal_color)( IMGINFO, DECDATA);

