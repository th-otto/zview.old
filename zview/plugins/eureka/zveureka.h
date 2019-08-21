#include "general.h"
boolean __CDECL reader_init( const char *name, IMGINFO info);
boolean __CDECL reader_read( IMGINFO info, uint8_t *buffer);
void    __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    __CDECL reader_quit( IMGINFO info);
#ifdef PLUGIN_SLB
#include "zvplugin.h"
long __CDECL get_option(zv_int_t which);
#endif

#define VERSION 0x203
