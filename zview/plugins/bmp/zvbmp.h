boolean __CDECL reader_init( const char *name, IMGINFO info);
boolean __CDECL reader_read( IMGINFO info, uint8 *buffer);
void    __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    __CDECL reader_quit( IMGINFO info);
