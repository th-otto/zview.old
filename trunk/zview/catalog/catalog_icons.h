extern int16 icons_init( void);
extern void icons_exit(void);
extern void set_entry_icon( Entry *entry);
extern void redraw_icon( WINDOW *wind, Entry *entry);
extern void draw_icon( int16 handle, Entry *entry, boolean selected, RECT16 *dst_rect);
extern boolean icon_is_visible( Entry *entry, int16 window_height);

extern IMAGE icon_image;
extern IMAGE icon_file;
extern IMAGE icon_folder;
extern IMAGE icon_tos;
extern IMAGE icon_pdf;

extern MFDB mini_hdd;
extern MFDB mini_folder;
extern MFDB mini_big;
extern MFDB mini_big_on;		
extern MFDB mini_small;
extern MFDB mini_small_on;
extern MFDB mini_forward;
extern MFDB mini_forward_on;	
extern MFDB mini_back;
extern MFDB mini_back_on;
extern MFDB mini_up;
extern MFDB mini_up_on;
extern MFDB mini_reload;
extern MFDB mini_reload_on;
extern MFDB mini_printer;
extern MFDB mini_printer_on;
extern MFDB mini_info;
extern MFDB mini_info_on;
extern MFDB mini_find;
extern MFDB mini_find_on;
extern MFDB mini_delete;
extern MFDB mini_delete_on;
extern MFDB mini_fullscreen;
extern MFDB mini_fullscreen_on;	  
extern MFDB mini_open;
extern MFDB mini_open_on;	
extern MFDB mini_save;
extern MFDB mini_save_on;


