/*
 * functions that can be called by the application
 */
#include <mint/slb.h>

boolean __CDECL plugin_reader_init( SLB *slb, const char *name, IMGINFO info);
boolean __CDECL plugin_reader_read( SLB *slb, IMGINFO info, uint8 *buffer);
void    __CDECL plugin_reader_get_txt( SLB *slb, IMGINFO info, txt_data *txtdata);
void    __CDECL plugin_reader_quit( SLB *slb, IMGINFO info);

long __CDECL plugin_slb_control(SLB *slb, long fn, void *arg);

#define plugin_compile_flags(slb) plugin_slb_control(slb, 0, 0)
#define plugin_set_imports(slb, f) plugin_slb_control(slb, 1, f)
#define plugin_get_basepage(slb) ((BASEPAGE *)plugin_slb_control(slb, 2, 0))
#define plugin_get_header(slb) plugin_slb_control(slb, 3, 0)
#define plugin_get_libpath(slb) ((const char *)plugin_slb_control(slb, 4, 0))
