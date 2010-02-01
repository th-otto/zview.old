#ifndef CFONT_H
#define CFONT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void draw_text( int16 vdih, int16 xf, int16 yf, int16 color, const char *str);
extern int16 get_text_width( const char *str);
extern int16 name_shorter( int16 max_size, char *str);

#ifdef __cplusplus
}
#endif

#endif

