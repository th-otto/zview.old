#define WD_DATA		0x4E414D45L

typedef struct _bookm
{
	char 			name[MAXNAMLEN];		/*  Folder's name shown in the window	*/

	RECT16 			arrow_position;			/* icon position in the window			*/

	struct 	_bookm	*parent;				/* the parent entry						*/
	struct 	_bookm	*child;					/* the child(s) entry					*/

	int16 			nbr_child;				/*  number of valid child's entries		*/
	int16 			txt_width;				/*  Text width for screen output		*/
	int16			state;					/*  Deployed or not ( ON/OFF)			*/
	int16			linked_page;			
	int16			valid;						
} Bookmark;

typedef struct
{
	int8		name[MAXNAMLEN+MAX_PATH];	/* Window's name text buffer		*/
	int8		info[256];					/* Window's infobar text buffer		*/
	int8		title[256];					/* document's title text buffer		*/

	int16		zoom_level;					/* Allowed values:
											25, 50, 100, 150, 200, fit the window */ 
	int16		pause;						/* make a "pause" in the animation	*/
	uint16		page_to_show;				/* Page to show in the window		*/
	clock_t 	chrono_value;				/* for animation function			*/

	IMAGE 		img;						/* the image structure				*/
	MFDB		icon;						/* the icon for the iconify mode	*/
	MFDB		zoom_picture;				/* the zoomed picture				*/

	/* The following data is for the bookmark's frame.. 	*/
	int16		frame_width;				/* first frame width if any			*/
	int16		border_width;				/* frame border's width if any		*/
	int32       ypos;   	 			 	/* relative data position in the window 	*/
	int32       ypos_max;	 				/* Maximal values of ypos variables		 	*/
	int16       h_u;    	   				/* vertical scroll offset					*/
	OBJECT 		*frame_slider;				/* vertical slider OBJECT if any			*/
	Bookmark	*selected;			
	Bookmark	*root;		
	int16 		nbr_bookmark;				/* Number of valid root's bookmarks	*/
}WINDATA;

extern WINDOW *WindView( char *filename);
extern void WindViewIcon( WINDOW *win);
extern void Win_VSlide( WINDOW *win);
extern void Win_HSlide( WINDOW *win);
extern void Win_DownPage( WINDOW *win);
extern void Win_RightPage( WINDOW *win);
extern void Win_LeftPage( WINDOW *win);
extern void Win_UpPage( WINDOW *win);
extern void Win_UpLine( WINDOW *win);
extern void Win_LeftLine( WINDOW *win); 
extern void Win_RightLine( WINDOW *win);
extern void Win_DownLine( WINDOW *win);
extern void Win_Arrow( WINDOW *win);

