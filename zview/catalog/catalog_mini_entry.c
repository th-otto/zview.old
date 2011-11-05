#include "../general.h"
#include "../prefs.h"
#include "../custom_font.h"
#include "catalog_icons.h"
#include "catalog_entry.h"
#include "../file/file.h"
#include "catalog.h"

/* Prototype */
void delete_mini_entry_child( Mini_Entry *entry);
void redraw_mini_entry( WINDOW *win, Mini_Entry *entry);
void draw_mini_entry( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 xw, int16 yw, int16 hw);
int16 draw_mini_entry_child( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 x, int16 y, int16 xw, int16 yw, int16 hw);
int find_mini_entry_child_on_mouse( WINDOW *win, Mini_Entry *entry, int16 mouse_x, int16 mouse_y);
Mini_Entry *find_mini_entry_child_by_path( Mini_Entry *parent, char *path);
Mini_Entry *find_mini_entry_by_path( WINDICON *wicones, char *path);
void calc_mini_entry_slider( WINDICON *wicones, OBJECT *slider_root);

/* extern variable */
extern int16 need_frame_slider;


/*==================================================================================*
 * delete_mini_entry_child:															*
 *		Free the memory used by all the 'child' items of a mini entry and put the	*
 *		parent 'nbr_child' counter to zero.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		entry	= the entry parent.													*
 *----------------------------------------------------------------------------------*
 * return: 																			*
 *      --																			*
 *==================================================================================*/

void delete_mini_entry_child( Mini_Entry *entry)
{
	int i;
	
	/* if nothing to do, end the function */
	if( entry->child == NULL)
		return;

	/* make a loop to see if the childs mini entries have child, if it's true, delete it */
	for ( i = 0; i < entry->nbr_child ; i++)
	{
		if ( entry->child[i].nbr_child)
			delete_mini_entry_child( &entry->child[i]);
	}

	/* Free the memory and put the counter to zero */
	gfree( entry->child);
	entry->child 		= NULL;
	entry->nbr_child 	= 0;
}


/*==================================================================================*
 * redraw_mini_entry:																*
 *		Send a redraw event for a mini_entry in the WINDOW *win.					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win     = the target window													*
 * 		entry	= the icon to be redraw												*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		--																			*
 *==================================================================================*/

void redraw_mini_entry( WINDOW *win, Mini_Entry *entry)
{
	int16 xw, yw, hw, x, y, w, h;

	WindGet ( win, WF_WORKXYWH, &xw, &yw, &w, &hw);

	x = entry->arrow_position.x1;
	y = entry->icon_position.y1;
	w = entry->icon_position.x2 - entry->icon_position.x1 + 20;
	h = entry->icon_position.y2 - y + 1;

	x += xw;
	y += yw;

	/* send the redraw event only if the mini entry is visible */
	if (( y + entry->icon_position.y2 > yw) && ( y < yw + hw))
		draw_page( win, x, y, MIN( w, browser_frame_width), h);
}


/*==================================================================================*
 * draw_mini_entry:																	*
 *		draw a mini_entry in the WINDOW *win.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		handle	 = the window's VDI handle.											*
 * 		selected = the mini_entry to be draw.										*
 * 		entry    = pointer to the mini_entry selected.								*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		--																			*
 *==================================================================================*/

void draw_mini_entry( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 x, int16 y, int16 h)
{
	MFDB 	*icon, screen = {0};
	int16	xy[8], xtext, ytext;

	
	if( ( entry->icon_position.y1 + y) < y || ( entry->icon_position.y1 + y) >= y + h)
		return;
	
	/* DRAW ARROW IS NEEDED */
	if ( entry->state != UNKNOWN)
	{
		if ( entry->state == ON)
		{
			xy[0] =	entry->arrow_position.x1 + x; 
			xy[1] = entry->arrow_position.y1 + 3 + y; 
			xy[2] = xy[0] + 10;
			xy[3] = xy[1];
			xy[4] = xy[0] + 5;
			xy[5] = xy[1] + 5;
		}
		else
		{
			xy[0] =	entry->arrow_position.x1 + x + 4; 
			xy[1] = entry->arrow_position.y1 + y; 
			xy[2] = xy[0];
			xy[3] = xy[1] + 10;
			xy[4] = xy[0] + 5;
			xy[5] = xy[1] + 5;		
		}

		vsf_color( win->graf.handle, LBLACK);
		v_fillarea( win->graf.handle, 3, xy);
	}


	/* DRAW ICON */
	if ( entry->parent) 
		icon = &mini_folder;
	else
	{
		switch( entry->name[0])
		{
			case 'A':
			case 'B':
				icon = &mini_hdd;	
				break;
			
			default:
				icon = &mini_hdd;	
				break;
		}	
	}

	xy[0] = 0;
	xy[1] = 0;
	xy[2] = 15;
	xy[3] = 15;
	xy[4] = entry->icon_position.x1 + x;
	xy[5] = entry->icon_position.y1 + y;
	xy[6] = xy[4] + 15;
	xy[7] = xy[5] + 15;

	if ( icon->fd_nplanes == 1)
	{
		int16	color[2];
		
		color[0] = WHITE;
		color[1] = BLACK;

		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, icon, &screen, color);
	}
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, icon, &screen);
		

	/* DRAW TEXT */
	xtext = entry->icon_position.x1 + x + 15 + x_space;				/* column x position + arrow + icon + space */
	ytext = entry->icon_position.y1 + y + 4;		/* line x postion + 1 + (( line height - font height) / 2) */
		
	if ( entry == selected)
	{
		xy[0] = xtext - 2;
		xy[2] = xy[0] + entry->icon_txt_w + 4;
		xy[1] = ytext - 1;
		xy[3] = xy[1] + hcell + 2;

		vsf_color( win->graf.handle, BLACK);
		v_bar( win->graf.handle, xy);

		draw_text( win->graf.handle, xtext, ytext, WHITE, entry->name);
	}
	else
		draw_text( win->graf.handle, xtext, ytext, BLACK, entry->name);
}



/*==================================================================================*
 * calc_mini_entry_line:															*
 *		compute the number of line needed for entry's mini_entries's childs.		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		entry    = The parent mini_entry.											*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		number of lines needed														*
 *==================================================================================*/

int16 calc_mini_entry_line( Mini_Entry *entry)
{
	int16 i, lines = 0;

	if( entry->child == NULL)
		 return lines;	

	for ( i = 0; i < entry->nbr_child ; i++)
	{
		lines++;
		lines += calc_mini_entry_line( &entry->child[i]);
	}

	return lines;
}



/*==================================================================================*
 * draw_mini_entry_child:															*
 *		draw every mini_entry's childs in the WINDOW *win.							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		 = the window where to draw.										*
 * 		selected = pointer to the mini_entry selected.								*
 * 		entry    = The parent mini_entry.											*
 * 		x 		 = x window's position.												*
 * 		y 		 = y window's position.												*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		number of lines drawn														*
 *==================================================================================*/

int16 draw_mini_entry_child( WINDOW *win, Mini_Entry *selected, Mini_Entry *entry, int16 x, int16 y, int16 xw, int16 yw, int16 hw)
{
	int16 i, dum, lines = 0;

	if( entry->child == NULL)
		 return lines;	

	for ( i = 0; i < entry->nbr_child ; i++)
	{
		entry->child[i].arrow_position.x1 = x + 3;
		entry->child[i].arrow_position.y1 = y + 3;
		entry->child[i].arrow_position.x2 = entry->child[i].arrow_position.x1 + 11;
		entry->child[i].arrow_position.y2 = entry->child[i].arrow_position.y1 + 11;

		entry->child[i].icon_position.x1 = x + 15;
		entry->child[i].icon_position.y1 = y + 1;
		entry->child[i].icon_position.x2 = entry->child[i].icon_position.x1 + 15 + x_space + entry->child[i].icon_txt_w;
		entry->child[i].icon_position.y2 = entry->child[i].icon_position.y1 + 15;

		draw_mini_entry( win, selected, &entry->child[i], xw, yw, hw);
		lines++;

		y += 18;

		dum = draw_mini_entry_child( win, selected, &entry->child[i], x + 15, y, xw, yw, hw);

		y += ( dum * 18);

		lines += dum;
	}

	return lines;
}



/*==================================================================================*
 * find_mini_entry_child_on_mouse:													*
 *		Look if the mouse click is on a entry's childs or on childs's arrow.		*
 *		If yes, open the mini_entry if the mouse is on the arrow or enter in the 	*
 *		directory seleted and send a redraw event to the main frame if is on the	* 
  *		entry itself.																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win     = the target window.												*
 * 		entry	= the mini_entry where search for childs'entry selected.			*
 *		mouse_x, mouse_y = the mouse position.										*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		1 if the mini_entry selected is found or 0.									*
 *==================================================================================*/

int find_mini_entry_child_on_mouse( WINDOW *win, Mini_Entry *entry, int16 mouse_x, int16 mouse_y)
{
	int i;

	if( entry->child == NULL)
		 return( 0);

	for ( i = 0; i < entry->nbr_child ; i++)
	{
		if ( entry->child[i].nbr_child > 0)
			if ( find_mini_entry_child_on_mouse( win, &entry->child[i], mouse_x, mouse_y))
				return ( 1);
	
		if  (( mouse_x >= entry->child[i].icon_position.x1 && mouse_x <= entry->child[i].icon_position.x2 && mouse_y >= entry->child[i].icon_position.y1 && mouse_y <= entry->child[i].icon_position.y2))
		{
			WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);

			if ( wicones->mini_selected != &entry->child[i])
			{	
				Mini_Entry	*old_selected = wicones->mini_selected;

				wicones->mini_selected = &entry->child[i];
					
				if ( old_selected)
					redraw_mini_entry( win, old_selected);
				
				redraw_mini_entry( win, wicones->mini_selected);	

				if ( strcmp ( wicones->mini_selected->foldername, wicones->directory) != 0)
				{   
					int16 x, y, w, h;
					
					if ( wicones->first_selected)
					{
						while ( wicones->first_selected)
							remove_selected_entry( wicones, wicones->first_selected);  
					} 

					graf_mouse( BUSYBEE, NULL);	
				
					( void)scan_dir( win, wicones->mini_selected->foldername);
					WindSetStr ( win, WF_NAME, wicones->directory);
					wicones->first_selected =  NULL;
					win->ypos 				=  0;
				
					WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);	   
					draw_page( win, x + browser_frame_width, y, w - browser_frame_width, h);   

					graf_mouse( ARROW, NULL);
				
					WindMakePreview( win);				
				}
			}
			return( 1);
		}
		else if ( entry->child[i].state != UNKNOWN)
		{
			if (( mouse_x >= entry->child[i].arrow_position.x1 && mouse_x <= entry->child[i].arrow_position.x2 && mouse_y >= entry->child[i].arrow_position.y1	&& mouse_y <= entry->child[i].arrow_position.y2))
			{
				int16 x, y, h, w;
				
				WindGet( win, WF_WORKXYWH, &x, &y, &w, &h); 
				
				if ( entry->child[i].state == ON)
				{
					entry->child[i].state = OFF;
					delete_mini_entry_child( &entry->child[i]);
				}
				else
				{
					Mini_Entry	*selected = &entry->child[i];
			
					graf_mouse( BUSYBEE, NULL);	
						
					selected->state = ON;
					scan_mini_dir( win, selected);
					check_mini_dir( selected->nbr_child, selected->child);
						
					graf_mouse( ARROW, NULL);
				}

				WinCatalog_Refresh( win);
				draw_page( win, x, y, browser_frame_width, h);   

				return( 1);
			}
		}
	}
	return( 0);
}



/*==================================================================================*
 * find_mini_entry_child_by_path:													*
 *		find a child mini_entry in parent and return it.							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		parent  = the parent where to search.										*
 * 		path	= the path of the mini_entry wanted.								*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		A pointer to the mini_entry if found else NULL.								*
 *==================================================================================*/
 
Mini_Entry *find_mini_entry_child_by_path( Mini_Entry *parent, char *path)
{
	int i;

	Mini_Entry	*entry = NULL;

	if( parent->child == NULL)
		return NULL;
		
	/* make a loop to see if the childs mini entries have child, if it's true, check it */
	for ( i = 0; i < parent->nbr_child ; i++)
	{
		if ( parent->child[i].nbr_child)
		{
			if ( ( entry = find_mini_entry_child_by_path( &parent->child[i], path)))
				return entry;
		}
			
		if ( strcmp ( parent->child[i].foldername, path) == 0)
			return &parent->child[i];
	}

	return NULL;
}


/*==================================================================================*
 * find_mini_entry_by_path:															*
 *		find a mini_entry and return it.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		wicones  = the WINDICON struct where to search.								*
 * 		path	 = the path of the mini_entry wanted.								*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		A pointer to the mini_entry if found else NULL.								*
 *==================================================================================*/
 
Mini_Entry *find_mini_entry_by_path( WINDICON *wicones, char *path)
{
	Mini_Entry	*entry = NULL;
	int16		i;
	
	for ( i = 0; i < wicones->nbr_child; i++)
	{
		if ( wicones->root[i].nbr_child)
		{
			if ( ( entry = find_mini_entry_child_by_path( &wicones->root[i], path)))
				return entry;
		}
			
		if ( strcmp ( wicones->root[i].foldername, path) == 0)
			return &wicones->root[i];
	}

	return NULL;
}




/*==================================================================================*
 * recalc_mini_entry_child_txt_width:												*
 *		static function called by recalc_mini_entry_txt_width()						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		wicones  = the WINDICON struct where to search.								*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		--																			*
 *==================================================================================*/
 
static void recalc_mini_entry_child_txt_width( Mini_Entry *parent)
{
	int i;

	if( parent->child == NULL)
		return;
		
	/* make a loop to see if the childs mini entries have child, if it's true, make the job */
	for ( i = 0; i < parent->nbr_child; i++)
	{
		if ( parent->child[i].nbr_child)
			recalc_mini_entry_child_txt_width( &parent->child[i]);

		parent->child[i].icon_txt_w = get_text_width( parent->child[i].name);
		parent->child[i].icon_position.x2 = parent->child[i].icon_position.x1 + 15 + x_space + parent->child[i].icon_txt_w;
	}
}


/*==================================================================================*
 * recalc_mini_entry_txt_width:														*
 *		After a font change, we need to recalc the mini_entry text width.			*
 *		This function do it.														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		wicones  = the WINDICON struct where to search.								*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		--																			*
 *==================================================================================*/
 
void recalc_mini_entry_txt_width( WINDICON *wicones)
{
	int16 i;
	
	for ( i = 0; i < wicones->nbr_child; i++)
	{
		if ( wicones->root[i].nbr_child)
			recalc_mini_entry_child_txt_width( &wicones->root[i]);

		wicones->root[i].icon_txt_w = get_text_width( wicones->root[i].name);
		
		wicones->root[i].icon_position.x2 = wicones->root[i].icon_position.x1 + 15 + x_space + wicones->root[i].icon_txt_w;
	}
}



/*==================================================================================*
 * calc_mini_entry_slider:															*
 *		compute the frame slider size and coordinate.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		wicones  		= the target WINDICON struct.								*
 * 		slider_root	 	= the parent slider OBJECT.									*
 *----------------------------------------------------------------------------------*
 * return:																			*	
 *		--																			*
 *==================================================================================*/

void calc_mini_entry_slider( WINDICON *wicones, OBJECT *slider_root)
{
	int16 max_mover_size 	= slider_root[SLIDERS_BACK].ob_height;
	int16 full_win_size  	= wicones->ypos_max * wicones->h_u;
	int16 win_h 			= slider_root->ob_height + 3;

	if ( win_h >= full_win_size)
	{	
		wicones -> ypos = 0;	
					
		slider_root[SLIDERS_MOVER].ob_y 		= 0;	
		slider_root[SLIDERS_MOVER].ob_height  	= max_mover_size;
	
	}										
	else
	{	
		int16 ligne_reste;
		float mover_position	= 0L;
		float position_step		= 0L;
		float mover_size 		= MAX( ( float)max_mover_size * ( ( float)win_h / ( float)full_win_size), 10L);

		if( wicones->ypos)
		{
			ligne_reste		= wicones->ypos_max - ( win_h / wicones->h_u);
			position_step	= (( float)max_mover_size - mover_size) / ( float)ligne_reste;
			mover_position 	= position_step * ( float)wicones->ypos;

			while( (( int16)mover_size + ( int16)mover_position > max_mover_size) && wicones->ypos > 0)
			{
				wicones->ypos--;
				mover_position 	-= position_step;
			}
		}	
			
		slider_root[SLIDERS_MOVER].ob_height  	= ( int16)mover_size;
		slider_root[SLIDERS_MOVER].ob_y 		= ( int16)mover_position;
	}	
}
