extern void draw_text( int16 vdih, int16 xf, int16 yf, int16 color, const char *str);
extern inline void draw_text_to_buffer( int16 xf, int16 yf, int32 src_line_octets, const char *str, MFDB *bm);
extern inline int16 get_text_width( const char *str);
extern int16 name_shorter( int16 max_size, char *str);
extern char *zstrncpy( char *dst, const char *src, size_t n);
extern void trim_start( char *name);
extern void trim_end( char *name, int16 len);




