extern boolean dir_cd( const char *new_dir, char *old_dir, int16 bufflen);
extern boolean dir_parent( char *dir, char *old_dir);
extern boolean check_write_perm( const struct stat *file_stat); 
extern boolean scan_dir( WINDOW *win, const char *dirpath);
extern boolean scan_mini_drv( WINDOW *win);
extern void check_mini_dir( int16 nb_child, Mini_Entry *entry);
extern boolean scan_mini_dir( WINDOW *win, Mini_Entry *parent);
