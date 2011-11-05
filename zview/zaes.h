extern OBJECT *get_tree( int16 obj_index);
extern char *get_string( int16 str_index);
extern void errshow( const char *name, int16 error);
extern void generic_form_event( WINDOW *win);

extern void CDECL draw_icon_up( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_reload( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_greater( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_smaller( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_object_image( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_back( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_forward( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_printer( WINDOW *win, PARMBLK *pblk, void *data); 
extern void CDECL draw_icon_open( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_info( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_delete( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_save( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_fullscreen( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_icon_find( WINDOW *win, PARMBLK *pblk, void *data);

extern void draw_window_iconified( WINDOW *win);
