extern void delete_mini_entry_child( Mini_Entry *entry);
extern int find_mini_entry_child_on_mouse( WINDOW *win, Mini_Entry *entry, int16 mouse_x, int16 mouse_y);
extern void redraw_mini_entry( WINDOW *win, Mini_Entry *entry);
extern void draw_mini_entry( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 x, int16 y, int16 h);
extern int16 draw_mini_entry_child( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 x, int16 y, int16 xw, int16 yw, int16 hw);
extern Mini_Entry *find_mini_entry_by_path( WINDICON *wicones, char *path);
extern Mini_Entry *find_mini_entry_child_by_path( Mini_Entry *parent, char *path);
extern void calc_mini_entry_slider( WINDICON *wicones, OBJECT *slider_root);
extern int16 calc_mini_entry_line( Mini_Entry *entry);
extern void recalc_mini_entry_txt_width( WINDICON *wicones);