extern void WinCatalog_Arrow( WINDOW *win);
extern void WinCatalog_VSlide( WINDOW *win);
extern void WinCatalog_DownLine( WINDOW *win);
extern void WinCatalog_UpLine( WINDOW *win);
extern void WinCatalog_DownPage( WINDOW *win);
extern void WinCatalog_UpPage( WINDOW *win);

extern void move_main_work( WINDOW *win, int16 xw, int16 yw, int16 ww, int16 hw, int16 dx, int16 dy, int16 first_frame_width, int16 border_width);
extern void move_frame_work( WINDOW *win, int16 dy);
extern void move_area( int16 handle, GRECT *screen, int16 dx, int16 dy);

