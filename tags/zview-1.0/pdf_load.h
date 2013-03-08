extern int16 pdf_initialised;
extern int16 read_pdf( IMAGE *img, int16 page, double scale);
extern int16 pdf_load( const char *file, IMAGE *img, uint16 width, uint16 height);
extern double get_scale_value( IMAGE *img, int16 page, uint16 target_width, uint16 target_height);
