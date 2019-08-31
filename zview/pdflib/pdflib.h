#ifdef __cplusplus
extern "C" {
#endif

/*
 * these are part of the shared library
 */
boolean __CDECL pdf_init(const char *path);
void __CDECL pdf_exit(void);
boolean __CDECL lib_pdf_load(const char *name, IMAGE *img, boolean antialias);
void __CDECL pdf_get_page_size(IMAGE *img, int page, uint16 *width, uint16 *height);
void __CDECL pdf_decode_page(IMAGE *img, int page, double scale);
uint32 *__CDECL pdf_get_page_addr(IMAGE *img);
void __CDECL pdf_quit(IMAGE *img);
void __CDECL pdf_build_bookmark(WINDATA *windata, WINDOW *win);
void __CDECL pdf_get_info(IMAGE *img, txt_data *txtdata);
void __CDECL delete_bookmarks(WINDATA *windata);


long zvpdf_freetype_open(void);
void zvpdf_freetype_close(void);

/*
 * these are part of the application
 */
long zvpdf_open(void);
void zvpdf_close(void);

extern int16 (*p_get_text_width)(const char *str);

#ifdef __cplusplus
}
#endif
