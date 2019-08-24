/*
 * functions that are called by the application
 */
#include <mint/slb.h>
#include "imginfo.h"

extern char *zview_slb_dir;
extern char *zview_slb_dir_end;

long plugin_open(const char *name, const char *path, SLB *slb);
void plugin_close(SLB *slb);

boolean __CDECL plugin_reader_init(SLB *slb, const char *name, IMGINFO info);
boolean __CDECL plugin_reader_read(SLB *slb, IMGINFO info, uint8_t *buffer);
void __CDECL plugin_reader_get_txt(SLB *slb, IMGINFO info, txt_data *txtdata);
void __CDECL plugin_reader_quit(SLB *slb, IMGINFO info);
boolean __CDECL plugin_encoder_init(SLB *slb, const char *name, IMGINFO info);
boolean __CDECL plugin_encoder_write(SLB *slb, IMGINFO info, uint8_t *buffer);
void __CDECL plugin_encoder_quit(SLB *slb, IMGINFO info);

/*
 * Parameters to get_option()/set_option()
 * OPTION_CAPABILITIES and OPTION_EXTENSIONS are
 * mandatory for get_option, others are optional
 */
#define OPTION_CAPABILITIES 0
#define OPTION_EXTENSIONS   1
#define OPTION_QUALITY      2
#define OPTION_COLOR_SPACE  3
#define OPTION_PROGRESSIVE  4
#define OPTION_COMPRESSION  5

/*
 * Flags for get_option(OPTION_CAPABILITIES)
 */
#define CAN_DECODE 0x01
#define CAN_ENCODE 0x02

long __CDECL plugin_get_option(SLB *slb, zv_int_t which);
long __CDECL plugin_set_option(SLB *slb, zv_int_t which, zv_int_t value);

/*
 * internal functions used while loading libraries
 */
long __CDECL plugin_slb_control(SLB *slb, long fn, void *arg);

#define plugin_compile_flags(slb) plugin_slb_control(slb, 0, 0)
#define plugin_set_imports(slb, f) plugin_slb_control(slb, 1, f)
#define plugin_get_basepage(slb) ((BASEPAGE *)plugin_slb_control(slb, 2, 0))
#define plugin_get_header(slb) plugin_slb_control(slb, 3, 0)
#define plugin_get_libpath(slb) ((const char *)plugin_slb_control(slb, 4, 0))
#define plugin_required_libs(slb) ((const char *)plugin_slb_control(slb, 5, 0))

/*
 * callback functions used to load other shared libs like zlib etc.
 */
long __CDECL plugin_slb_open(zv_int_t lib);
void __CDECL plugin_slb_close(zv_int_t lib);
SLB *__CDECL plugin_slb_get(zv_int_t lib);
