boolean __CDECL reader_init( const char *name, IMGINFO info);
boolean __CDECL reader_read( IMGINFO info, uint8 *buffer);
void __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void __CDECL reader_quit( IMGINFO info);
boolean __CDECL encoder_init(const char *name, IMGINFO info);
boolean __CDECL encoder_write(IMGINFO info, uint8 *buffer);
void __CDECL encoder_quit(IMGINFO info);
#ifdef PLUGIN_SLB
#include "zvplugin.h"
long __CDECL get_option(zv_int_t which);
long __CDECL set_option(zv_int_t which, zv_int_t value);
#endif

void jpg_init(void);

/* Options*/
extern int quality; /* quality 0 -> 100 % */
extern J_COLOR_SPACE color_space;
extern boolean progressive;
