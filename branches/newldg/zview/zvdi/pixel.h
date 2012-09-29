extern void  ( *getPixel)( int, int, uint32, uint8*, uint8*, uint8*, uint8*);
extern void  ( *setPixel)( int, int, uint32, uint8, uint8, uint8, uint8*);

extern void getPixel_32( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_32( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_32r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_32r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_32z( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_32z( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_24( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_24( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_24r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_24r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_16( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_16( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);
extern void getPixel_16r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src);
extern void setPixel_16r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst);


