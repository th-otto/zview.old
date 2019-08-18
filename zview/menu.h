extern char const program_version[];
extern char const program_date[];

extern void MenuDesktop( void);
extern void __CDECL Menu_open_image(WINDOW *win, int item, int title, void *data);
void save_dialog( const char *fullfilename);

unsigned int get_option_mask(CODEC *codec);
void save_option_dialog(const char *source_file, CODEC *codec);
void tiff_option_dialog( void);
