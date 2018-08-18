void __CDECL WinCatalog_Arrow( WINDOW *win EVNT_BUFF_PARAM);
void __CDECL WinCatalog_VSlide( WINDOW *win EVNT_BUFF_PARAM);
void __CDECL WinCatalog_DownLine( WINDOW *win EVNT_BUFF_PARAM);
void __CDECL WinCatalog_UpLine( WINDOW *win EVNT_BUFF_PARAM);
void __CDECL WinCatalog_DownPage( WINDOW *win EVNT_BUFF_PARAM);
void __CDECL WinCatalog_UpPage( WINDOW *win EVNT_BUFF_PARAM);

extern void move_main_work( WINDOW *win, int16 xw, int16 yw, int16 ww, int16 hw, int16 dx, int16 dy, int16 first_frame_width, int16 border_width EVNT_BUFF_PARAM);
extern void move_frame_work( WINDOW *win, int16 dy EVNT_BUFF_PARAM);
extern void move_area( int16 handle, GRECT *screen, int16 dx, int16 dy);
