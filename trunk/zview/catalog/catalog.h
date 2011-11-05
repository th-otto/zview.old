extern int WinCatalog( void);
extern void WinCatalog_Close( WINDOW *win);
extern void WinCatalog_Refresh( WINDOW *wind);
extern void WindMakePreview( WINDOW *win);
extern int WindMakePreview_needed( WINDOW *win);
extern void WinCatalog_filelist_redraw( void);
extern void WinCatalog_set_thumbnails_size( WINDOW *win);

extern int16 	old_browser_size;
extern int16	old_preview_area_size;
extern int16 	x_space;
extern int16 	y_space;
extern int16 	hcell;
extern int16 	border_size;
extern int16 	old_border_size;
extern int16	need_frame_slider;
extern int16 	draw_frame_slider;
extern OBJECT 	*frame_slider_root;

