#include "../general.h"
#include "../ztext.h"
#include "catalog_entry.h"
#include "catalog_mini_entry.h"
#include "../prefs.h"
#include "../infobox.h"
#include "../pic_load.h"
#include "../winimg.h"
#include "../zedit/zedit.h"
#include "../file/file.h"
#include "catalog.h"
#include "catalog_icons.h"
#include "catalog_slider.h"
#include "catalog_popup.h"

/* Local variable */
static int16 	dum;
extern char 	fullname[MAX_PATH+MAXNAMLEN];

/* Prototype */
void WinCatalog_Mouse( WINDOW *win); 

/*==================================================================================*
 * void WinCatalog_Mouse:															*
 *		this function handle the mouse event for the main frame in the				* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The window to handle.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_Mouse( WINDOW *win) 
{
	WINDICON 	*wicones = (WINDICON *)DataSearch( win, WD_ICON);
	GRECT 		mouse;
	int16 		x, y, w, h, old_x, old_entry = 0, old_y, i = 0, nb_click, in_txt = 0, in_icn = 0, in_frame_border = 0, in_browser = 0; 
	Entry 		*entry_ptr = NULL, *old_selected = wicones->first_selected;
	EntryType	old_type = ET_FILE;
	
	if( old_selected)
	{
		old_entry = 1;
	
		old_type = old_selected->type;	
	}	
	
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h); 

 	mouse.g_x 				= evnt.mx - x;
	mouse.g_y 				= evnt.my - y;
	old_x     				= evnt.mx;
	old_y     				= evnt.my;
	nb_click  				= evnt.nb_click;

	if ( browser_frame_width && ( mouse.g_x >= wicones->border_position[0] && mouse.g_x <= wicones->border_position[1]))
	{	/* mouse on frame border */
		in_frame_border = 1;
	}
	else if ( browser_frame_width && ( mouse.g_x < wicones->border_position[0]))
	{	/* mouse on browser */
		in_browser = 1;
	}
	else
	{
		for ( i = 0 ; i < wicones->nbr_icons ; i++)	/* Check if the mouse is over a icon */
		{
			/* mouse on text */
			if  (( mouse.g_x >= wicones->entry[i].txt_pos.x1 && mouse.g_x <= wicones->entry[i].txt_pos.x2  && mouse.g_y >= wicones->entry[i].txt_pos.y1 && mouse.g_y <= wicones->entry[i].txt_pos.y2))
			{
				in_txt = 1;
				break;
			}
			/* click on case to select */
			else if (( mouse.g_x >= wicones->entry[i].case_pos.x1 && mouse.g_x <= wicones->entry[i].case_pos.x2 && mouse.g_y >= wicones->entry[i].case_pos.y1 && mouse.g_y <= wicones->entry[i].case_pos.y2))
			{
				in_icn = 1;
				break;
			}
		}
	}

	if(( evnt.mkstate & ( K_LSHIFT|K_RSHIFT)))
	{
		if( wicones->edit)
			exit_edit_mode( win, old_selected);

		/* If no icon or text icon are selected, we return */
		if( in_icn || in_txt) 
		{
			if( !check_selected_entry( wicones, &wicones->entry[i]))
				add_selected_entry( wicones, &wicones->entry[i]);
			else
				remove_selected_entry( wicones, &wicones->entry[i]);

			redraw_icon( win, &wicones->entry[i]);
		}
	}
	else if( in_icn)
	{
		if( wicones->edit)
			exit_edit_mode( win, old_selected);

		if( wicones->first_selected)
		{
			while( wicones->first_selected->next_selected)
			{
				entry_ptr = wicones->first_selected->next_selected;
				remove_selected_entry( wicones, wicones->first_selected->next_selected);
				redraw_icon( win, entry_ptr);
			}
		}

		wicones->first_selected = &wicones->entry[i];

		if( old_selected != wicones->first_selected)
		{
			if( old_selected)
				redraw_icon( win, old_selected);

			redraw_icon( win, wicones->first_selected);
		}
/*
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 

		if (( evnt.mbut == 1) && ( ( evnt.mx != old_x) || ( evnt.my != old_y)))
			move_icon( win, wicones);					
*/

		if ( nb_click == 2 && evnt.mbut == 1)
		{
			if ( wicones->first_selected->type == ET_DIR)
			{
				Mini_Entry	*old_selected_mini_entry = wicones->mini_selected;

				wicones->mini_selected = NULL;

				if ( old_selected_mini_entry && browser_frame_width)
					redraw_mini_entry( win, old_selected_mini_entry);

				strcat( wicones->directory, wicones->entry[i].name);
				( void)scan_dir( win, wicones->directory);

				WindSetStr ( win, WF_NAME, wicones->directory);
				wicones->first_selected =  NULL;
				win->ypos 				=  0;

				draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);   
				WindMakePreview( win);
			}
			else if ( ( wicones->first_selected->type == ET_IMAGE && wicones->first_selected->preview.page) || wicones->first_selected->type == ET_PDF)
			{
				strcpy( fullname, wicones->directory);
				strcat( fullname, wicones->first_selected->name);
				WindView( fullname); 

				// WindView( wicones->first_selected->name);
			}
		}
		else if( evnt.mbut == 2)
		{
			WinCatalog_Popup( win, wicones);
		}		
	}
	else if( in_txt)
	{
		if( wicones->first_selected)
		{
			while( wicones->first_selected->next_selected)
			{
				entry_ptr = wicones->first_selected->next_selected;
				remove_selected_entry( wicones, wicones->first_selected->next_selected);
				redraw_icon( win, entry_ptr);
			}
		}

		wicones->first_selected = &wicones->entry[i];

		if( old_selected != wicones->first_selected)
		{
			if( old_selected)
			{
				if( wicones->edit)
					exit_edit_mode( win, old_selected);
				else
					redraw_icon( win, old_selected);
			}

			redraw_icon( win, wicones->first_selected);
		}
		else if( !wicones->edit)
			init_edit_mode( win, wicones->first_selected);
	}
	else if ( in_frame_border && evnt.mbut == 1)
	{
		int16 last_mouse_x;

		if ( wicones->edit)
			exit_edit_mode( win, old_selected);		

		graf_mouse( FLAT_HAND, NULL);		

		while( !wind_update( BEG_MCTRL));

		graf_dragbox( border_size, h, x + browser_frame_width, y, x + 100, y, w - ( 100 + win -> w_u), h, &last_mouse_x, &dum );
			
		/* Set the new toolbar position */
		if ( last_mouse_x != x + browser_frame_width)
		{
			int16 ww;

			if ( last_mouse_x < x + 100)
				browser_frame_width = 100;
			else if ( last_mouse_x > x + ( w - win -> w_u))
				browser_frame_width =  w - win -> w_u;
			else
				browser_frame_width =  last_mouse_x - x;

			/* Set new minimum value */
			WindCalc( WC_BORDER, win, 0, 0, wicones->case_w + border_size + browser_frame_width, wicones->case_h, &dum, &dum, &ww, &dum);
			win -> w_min = ww;

			/* ensure that the ypos is inferior that it maximum size */	
			WinCatalog_Refresh( win);				
			
			i = 0;

			while ( i < win -> ypos_max - h / win -> h_u) 
				i ++;	
	
			if( win -> ypos > i)
			   win -> ypos = i;

			EvntRedraw( win);
			WindMakePreview( win);
		}

		wind_update( END_MCTRL);

		graf_mouse( ARROW, NULL);
	}
	else if ( in_browser)
	{
		if ( wicones->edit)
			exit_edit_mode( win, old_selected);

		if( need_frame_slider && ( mouse.g_x > browser_frame_width - 15) && ( mouse.g_x < browser_frame_width))
		{
			int16 res;

			frame_slider_root->ob_x = x + browser_frame_width - 15;
			frame_slider_root->ob_y = y + 2;			

			res = objc_find( frame_slider_root, SLIDERS_BOX, 2, evnt.mx, evnt.my);

			if( res != -1) 
			{
				int32 	pos, old_ypos = wicones->ypos;
				int16	dy, selected_object, page, redraw_arrow_slider = 0;

				switch ( res) 
				{
					case SLIDERS_UP:
						if( wicones -> ypos > 0L)
							ObjcWindChange( win, frame_slider_root, SLIDERS_UP, frame_slider_root->ob_x, frame_slider_root[SLIDERS_UP].ob_y + y, frame_slider_root[SLIDERS_UP].ob_width + 1, frame_slider_root[SLIDERS_UP].ob_height + 1, SELECTED);

						do 
						{
							if ( wicones -> ypos > 0L) 
							{
								wicones -> ypos--;
								move_frame_work( win, -wicones->h_u);
								redraw_arrow_slider	= 1;
							}  				

							graf_mkstate( &dum, &dum, &res, &dum);

						} while( res);

						if( redraw_arrow_slider)
						{
							/* We don't call ObjcWindChange to change the up arrow state because it seem not work with userdraw object
							   ObjcWindChange( win, frame_slider_root, SLIDERS_UP, frame_slider_root->ob_x, frame_slider_root[SLIDERS_UP].ob_y + y, frame_slider_root->ob_width, frame_slider_root->ob_height, ~SELECTED); */
							frame_slider_root[SLIDERS_UP].ob_state &= ~SELECTED;
							ObjcWindDraw( win, frame_slider_root, SLIDERS_UP, 1, frame_slider_root->ob_x, frame_slider_root[SLIDERS_UP].ob_y + y, frame_slider_root[SLIDERS_UP].ob_width + 1, frame_slider_root[SLIDERS_UP].ob_height + 1);
						}
						break;

					case SLIDERS_DOWN:
						do 
						{
							if (( wicones -> ypos < wicones -> ypos_max - h / wicones -> h_u) && ( wicones -> ypos_max > h / wicones -> h_u )) 
							{
								if( redraw_arrow_slider == 0)
									ObjcWindChange( win, frame_slider_root, SLIDERS_DOWN, frame_slider_root->ob_x, frame_slider_root[SLIDERS_DOWN].ob_y + y, frame_slider_root[SLIDERS_DOWN].ob_width + 1, frame_slider_root[SLIDERS_DOWN].ob_height + 1, SELECTED);

								wicones -> ypos++;
								move_frame_work( win, wicones->h_u);
								redraw_arrow_slider	= 1;
							}	 		

							graf_mkstate( &dum, &dum, &res, &dum);

						} while( res);

						if( redraw_arrow_slider)
						{
							/* We don't call ObjcWindChange to change the up arrow state because it seem not work with userdraw object */
							frame_slider_root[SLIDERS_DOWN].ob_state &= ~SELECTED;
							ObjcWindDraw( win, frame_slider_root, SLIDERS_DOWN, 1, frame_slider_root->ob_x, frame_slider_root[SLIDERS_DOWN].ob_y + y, frame_slider_root[SLIDERS_DOWN].ob_width + 1, frame_slider_root[SLIDERS_DOWN].ob_height + 1);
						}
						break;

					case SLIDERS_MOVER:
						if( app.aes4 & AES4_XGMOUSE) 
							graf_mouse( M_SAVE, 0L);
						
						graf_mouse( FLAT_HAND, NULL);
						
						while( !wind_update( BEG_MCTRL));
						
						res = graf_slidebox( frame_slider_root, SLIDERS_BACK, SLIDERS_MOVER, 1);
						
						wind_update( END_MCTRL);
	
						pos = ( int32)( wicones->ypos_max - h / wicones->h_u) * res / 1000L;	
	
						if ( pos < 0) 
							pos = 0;

						dy = ( int16)(( pos - wicones->ypos) * wicones->h_u);

						wicones->ypos =  pos;

						if( dy && ( old_ypos != wicones->ypos)) 
							move_frame_work( win, dy);

						if( app.aes4 & AES4_XGMOUSE)
							graf_mouse( M_RESTORE, 0L);

						graf_mouse( ARROW,NULL);

						break;	

					case SLIDERS_BACK:
						objc_offset( frame_slider_root, SLIDERS_MOVER, &dum, &dy);
						
						dum  			= ( evnt.my < dy) ? WA_UPPAGE : WA_DNPAGE;
						selected_object = ( evnt.my < dy) ? SLIDERS_UP : SLIDERS_DOWN;

						while( !wind_update( BEG_MCTRL));

						frame_slider_root[selected_object].ob_state |= SELECTED;

						if( dum == WA_DNPAGE)
						{
							do 
							{
								page = h / wicones -> h_u;

								if ( wicones -> ypos < wicones -> ypos_max - page) 
								{
									wicones -> ypos = MIN( wicones->ypos_max, wicones->ypos) + page;
									wicones -> ypos = MIN( wicones -> ypos, wicones -> ypos_max - page);
									dy = ( int16) (( wicones->ypos - old_ypos) * wicones->h_u);
									move_frame_work( win, dy);
								}

								graf_mkstate( &dum, &dum, &res, &dum);
							} while( res);
						}
						else
						{
							do 
							{
								if ( wicones -> ypos > 0L) 
								{
									pos = MAX( 0L, wicones->ypos - h / wicones->h_u);
									dy = ( int16) (( pos - wicones->ypos) * wicones->h_u);
									wicones->ypos = pos;
									move_frame_work( win, dy);
								}						   

								graf_mkstate( &dum, &dum, &res, &dum);
							} while( res);
						}						
					
						frame_slider_root[selected_object].ob_state &= ~SELECTED;
						
						ObjcWindDraw( win, frame_slider_root, selected_object, 1, x, y, w, h);

						wind_update( END_MCTRL);
						
						break;	

					default:
						break;
				}
			}
		}
		else
		{  
			for ( i = 0; i < wicones->nbr_child; i++)
			{
				if ( wicones->root[i].nbr_child > 0)
					if ( find_mini_entry_child_on_mouse( win, &wicones->root[i], mouse.g_x, mouse.g_y))
						break;

				if (( mouse.g_x >= wicones->root[i].icon_position.x1 && mouse.g_x <= wicones->root[i].icon_position.x2	&& mouse.g_y >= wicones->root[i].icon_position.y1 && mouse.g_y <= wicones->root[i].icon_position.y2))
				{
					if ( wicones->mini_selected != &wicones->root[i])
					{	
						Mini_Entry	*old_selected_mini_entry = wicones->mini_selected;
						wicones->mini_selected = &wicones->root[i];
						
						if ( old_selected_mini_entry)
							redraw_mini_entry( win, old_selected_mini_entry);
						
						redraw_mini_entry( win, wicones->mini_selected);
	
						if ( strcmp ( wicones->mini_selected->foldername, wicones->directory) != 0)
						{   						
							while ( wicones->first_selected)
								remove_selected_entry( wicones, wicones->first_selected);  
	
							graf_mouse( BUSYBEE, NULL);	
	
							( void)scan_dir( win, wicones->mini_selected->foldername);
							WindSetStr ( win, WF_NAME, wicones->directory);
							wicones->first_selected =  NULL;
							win->ypos 				=  0;
						   
							draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);  
		
							graf_mouse( ARROW, NULL);
	
							WindMakePreview( win);
						}
					}
					break;
				}
				else if ( wicones->root[i].state != UNKNOWN)
				{
					if (( mouse.g_x >= wicones->root[i].arrow_position.x1 && mouse.g_x <= wicones->root[i].arrow_position.x2 && mouse.g_y >= wicones->root[i].arrow_position.y1 && mouse.g_y <= wicones->root[i].arrow_position.y2))
					{
						if ( wicones->root[i].state == ON)
						{
							wicones->root[i].state = OFF;
							delete_mini_entry_child( &wicones->root[i]);
						}
						else
						{
							Mini_Entry	*selected = &wicones->root[i];
							
							graf_mouse( BUSYBEE, NULL);	
	
							selected->state = ON;
							scan_mini_dir( win, selected);
							check_mini_dir( selected->nbr_child, selected->child);
	
							graf_mouse( ARROW, NULL);
						}

						WinCatalog_Refresh( win);	

						draw_page( win, x, y , browser_frame_width, h);   
						   
						break;
					}
				}
			}
		}
	}
	else if ( !in_txt && !in_icn && !in_browser && !in_frame_border)
	{	
		if ( wicones->edit)
			exit_edit_mode( win, old_selected);

		while( wicones->first_selected)
		{
			entry_ptr = wicones->first_selected;
			remove_selected_entry( wicones, wicones->first_selected);
			redraw_icon( win, entry_ptr);
		}
	}

	if( !( wicones->first_selected))
	{
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 0);
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 0);
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_DELETE, 0);
		
		if( old_entry)
		{
			ObjcDraw( OC_TOOLBAR, win, TOOLBAR_DELETE, 1);
			ObjcDraw( OC_TOOLBAR, win, TOOLBAR_INFO, 1);
			
			if( old_type == ET_IMAGE)
				ObjcDraw( OC_TOOLBAR, win, TOOLBAR_SAVE, 1);		
		}
	}
	else
	{
		if( wicones->first_selected->type == ET_IMAGE)
			menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 1);

		menu_ienable( get_tree( MENU_BAR), MENU_BAR_DELETE, 1);
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 1); 
		
		if( !old_entry)
		{
			if( wicones->first_selected->type == ET_IMAGE)
				ObjcDraw( OC_TOOLBAR, win, TOOLBAR_SAVE, 1);
			
			ObjcDraw( OC_TOOLBAR, win, TOOLBAR_DELETE, 1);
			ObjcDraw( OC_TOOLBAR, win, TOOLBAR_INFO, 1);			
		}
		else if(( wicones->first_selected->type != old_type) && ( wicones->first_selected->type == ET_IMAGE || old_type == ET_IMAGE))		
			ObjcDraw( OC_TOOLBAR, win, TOOLBAR_SAVE, 1);
	}

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 				
}
