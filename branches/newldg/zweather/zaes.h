extern OBJECT *get_tree( int16 obj_index);
extern int8 *get_string( int16 str_index);
extern void errshow( const char *name, int16 error);
extern void generic_form_event( WINDOW *win);

extern void CDECL draw_weather_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast1_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast2_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast3_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast4_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast5_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_forecast6_icon( WINDOW *win, PARMBLK *pblk, void *data);
extern void CDECL draw_boussole_icon( WINDOW *win, PARMBLK *pblk, void *data);
