#include	"../general.h"
#include	"../catalog/catalog.h"
#include	"../zaes.h"
#include	"../custom_font.h"

/* prototype */
void cursor_position( WINDOW *win, int16 *x, int16 *y, int16 *w, int16 *h); 
void edit_icon_txt( WINDOW *win, Entry *entry);
void init_edit_mode( WINDOW *win, Entry *entry);
void exit_edit_mode( WINDOW *win, Entry *entry);


/*==================================================================================*
 * cursor_position:																	*
 * 		compute the cursor position in edition_mode.								*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *----------------------------------------------------------------------------------*
 * 	win:			The Target Window.												*
 * 	x, y, w, h:		Returned Cursor Positions.										*
 *==================================================================================*/

void cursor_position( WINDOW *win, int16 *x, int16 *y, int16 *w, int16 *h) 
{
    WINDICON *wicones 	= (WINDICON *)DataSearch( win, WD_ICON);
	CURSOR *cur			= &wicones->edit->cur;
	 
	WindGet( win, WF_WORKXYWH, x, y, w, h); 

	*h = 8;	
	
	if( cur->line) 
	{
		char txt[] = " ";
		char *p;

		*txt = cur -> line -> buf [ cur -> row];
		cur -> line -> buf [ cur -> row] = '\0';

		p = cur -> line -> buf;

		*x += get_text_width( p);
		cur -> line -> buf [ cur -> row] = *txt;

		*w = MAX( get_text_width( p), 8);
	} 
	else 
	{
		*w = 8;
		*x += cur -> row * 8;				/* Must be 0 ? */
	}
}





/*==================================================================================*
 * edit_icon_txt:																	*
 *		edit the selected icon 's text in real time in the WINDOW *win.				*
 *----------------------------------------------------------------------------------*
 * wind     		= the target window												*
 * entry 			= the selected entry to be edited								*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *==================================================================================*/

void edit_icon_txt( WINDOW *win, Entry *entry)
{
	int16 xw, yw, x, y, w, h;

	WindGet ( win, WF_WORKXYWH, &xw, &yw, &w, &h);

	x = entry->txt_pos.x1 - 5;
	y = entry->txt_pos.y1 - 1;
	w = entry->txt_pos.x2 + 10 - x;
	h = entry->txt_pos.y2 + 2 - y;

	x += xw;
	y += yw;

	draw_page( win, x, y, w, h);
}

/*==================================================================================*
 * init_edit_mode:																	*
 *		initialize the edition mode.												*
 *----------------------------------------------------------------------------------*
 * win     			= the target window												*
 * entry 			= the icon to be edited											*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *==================================================================================*/

void init_edit_mode( WINDOW *win, Entry *entry)
{
	WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON);

	if ( wicones->edit == NULL)
		wicones->edit = edit_new();

	string_put( wicones->edit, entry->name);

	WinCatalog_Refresh( win);

	edit_icon_txt( win, entry);
}



/*==================================================================================*
 * exit_edit_mode:																	*
 *		Exit edition mode and refresh the edited icon.								*
 *----------------------------------------------------------------------------------*
 * wind     		= the target window												*
 * entry 			= the last edited icon											*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *==================================================================================*/

void exit_edit_mode( WINDOW *win, Entry *entry)
{
    WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON);
	int16 	 xw, yw, x, y, w, h;

	WindGet( win, WF_WORKXYWH, &xw, &yw, &w, &h);

	x = entry->txt_pos.x1 - 5;
	y = entry->txt_pos.y1 - 1;
	w = entry->txt_pos.x2 + 10 - x;
	h = entry->txt_pos.y2 + 2 - y;

	x += xw;
	y += yw;

	edit_free( wicones->edit);
	wicones->edit = NULL;

	draw_page( win, x, y, w, h);
}

