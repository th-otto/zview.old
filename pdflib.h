boolean lib_pdf_load( const char *name, IMAGE *img);
void pdf_get_page_size( IMAGE *img, int page, uint16 *width, uint16 *height);
void pdf_decode_page( IMAGE *img, int page, double scale);
uint32 *pdf_get_page_addr( IMAGE *img);
void pdf_quit( IMAGE *img);
boolean pdf_init( char *path);
void pdf_exit( void);
char *get_pdf_title( void) ;
void pdf_build_bookmark( WINDATA *windata, WINDOW *win);
void pdf_get_info( IMAGE *img, txt_data *txtdata);
void delete_bookmark_child( Bookmark *book);


