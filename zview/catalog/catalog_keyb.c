#include "../general.h"
#include "../prefs.h"
#include "../zedit/zedit.h"
#include "../file/file.h"
#include "../file/count.h"
#include "../file/delete.h"
#include "../file/rename.h"
#include "../ztext.h"
#include "catalog_entry.h"
#include "catalog_mini_entry.h"
#include "catalog_icons.h"
#include "../pic_load.h"
#include "../winimg.h"
#include "catalog.h"
#include "../mfdb.h"
#include "../custom_font.h"

/* local constant */
static const char allowed_char[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZÇäàâáÄê°çãåóñ£ÅÖ†ÉÑÜëÅî§èí•0123456789 &\"'(›!)-_`ú^[]*{},?;.:/=+%#|@~<>";
char fullname[MAX_PATH+MAXNAMLEN];

/* Prototype */
void WinCatalog_Keyb( WINDOW *win);

// extern boolean create_icon( const char *filename, MFDB *icon, MFDB *icon_selected, MFDB *icon_mask);

/*==================================================================================*
 * void WinCatalog_Keyb:															*
 *		this function handle the keyboard event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The window to handle.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_Keyb( WINDOW *win) 
{
	WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON); 
	Mini_Entry	*old_selected;
	Entry *old_entry = wicones->first_selected;
	int16 	 res, selection, x, y, w, h, i;
	char 	 old_dir[128] = "";

	switch ( evnt.keybd >> 8)
	{
		case SC_UPARW:
		case SC_DWARW: 
		case SC_LFARW: 	
		case SC_RTARW:
		case SC_RETURN:
		case SC_ENTER:
		case SC_HOME: 
			if( wicones->first_selected)
			{
				if( wicones->first_selected->next_selected)
				{
					Entry *entry_ptr;				

					while( wicones->first_selected->next_selected)
					{
						entry_ptr = wicones->first_selected->next_selected;
						remove_selected_entry( wicones, wicones->first_selected->next_selected);
						redraw_icon( win, entry_ptr);
					}
					return;
				}
			}
			break;
		default:
			break;
	}

	WindGet ( win, WF_WORKXYWH, &x, &y, &w, &h); 	
	
	if ( !wicones->edit)
	{
		res = selection = return_entry_nbr( wicones, wicones->first_selected);
			   
		switch ( evnt.keybd >> 8)
		{
			case SC_UPARW: 		
				if ( selection == -1)
					selection = 0;	
				else if ( selection >= wicones->columns)
					selection = selection - wicones->columns;

				goto common;		
		
			case SC_DWARW: 
				if ( selection == -1)	
					selection = 0;	
				else if ( selection < wicones->nbr_icons - wicones->icons_last_line && selection + wicones->columns <  wicones->nbr_icons)
					selection = selection + wicones->columns;					

				goto common;		
	
			case SC_LFARW: 
				if ( selection == -1)	
					selection = 0;	
				else if ( selection > 0)
					selection--;			

				goto common;	

			case SC_RTARW:
				if ( selection == -1)	
					selection = 0;	
				else if ( selection < wicones->nbr_icons - 1)
					selection++;
					
				common:

				if ( wicones->entry[selection].icn_pos.y1 < 0)
				{	
					if ( wicones->entry[selection].icn_pos.y1 + wicones->case_h >= 0)
					{
						wicones->first_selected = &wicones->entry[selection];

						if ( res >= 0)
							redraw_icon( win, &wicones->entry[res]);

						snd_arrw( win, WA_UPLINE);
						/* WindMakePreview( win); */
 					}
					else 
					{
						while ( wicones->entry[selection].icn_pos.y1 <= 0)
						{
							if ( win->ypos == 0)
								break;
								
							wicones->entry[selection].icn_pos.y1 += wicones->case_h;								
							win->ypos--;
						}

						draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);	
						WindMakePreview( win);
						goto end_case;
					}	
				}	
				else if ( wicones->entry[selection].icn_pos.y2 > h)
				{
					if ( wicones->entry[selection].icn_pos.y2 - wicones->case_h <= h)	
					{
						wicones->first_selected = &wicones->entry[selection];

						snd_arrw( win, WA_DNLINE);

						if ( res >= 0)
						    redraw_icon( win, &wicones->entry[res]);

						redraw_icon( win, wicones->first_selected);	
					}						
					else 
					{
						while ( wicones->entry[selection].icn_pos.y2 >= h)
						{
							if ( win->ypos == win->ypos_max)
								break;
								
							wicones->entry[selection].icn_pos.y2 -= wicones->case_h;
							win->ypos++; 
						}
						draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);
						WindMakePreview( win);
						goto end_case;
					}	
				}	
				else if ( res != selection) 
				{
					wicones->first_selected = &wicones->entry[selection];
					
					if ( res >= 0)
					    redraw_icon( win, &wicones->entry[res]);

					redraw_icon( win, wicones->first_selected);		
				}

				end_case:

				break;

			case SC_RETURN:
			case SC_ENTER:
				if( !wicones->first_selected)
					break;		
				else if ( wicones->entry[selection].type == ET_DIR)
				{	
					Mini_Entry	*old_selected_mini_entry = wicones->mini_selected;

					wicones->mini_selected = NULL;

					if ( old_selected_mini_entry && browser_frame_width)
						redraw_mini_entry( win, old_selected_mini_entry);

					strcat( wicones->directory, wicones->entry[selection].name);
					( void)scan_dir( win, wicones->directory);
					WindSetStr ( win, WF_NAME, wicones->directory);
					wicones->first_selected =  NULL;
					win->ypos 				=  0;
					   
					draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);  
					WindMakePreview( win);
				}
				else if ( ( wicones->entry[selection].type == ET_IMAGE && wicones->entry[selection].preview.page) || wicones->entry[selection].type == ET_PDF)
				{	
					strcpy( fullname, wicones->directory);
					strcat( fullname, wicones->entry[selection].name);
					WindView( fullname); 
				}
				break;

			case SC_HOME: 
				if ( !dir_parent( wicones->directory, old_dir))
					break;
				
				old_selected = wicones->mini_selected;
				wicones->mini_selected = NULL;

				if ( old_selected)
					redraw_mini_entry( win, old_selected);
					
				( void)scan_dir( win, wicones->directory);
					
				WindSetStr ( win, WF_NAME, wicones->directory); 
				win->ypos =  0;
	
				for (i = 0 ; i < wicones->nbr_icons ; i++)	
					if ( !strcmp( wicones->entry[i].name, old_dir))
					{   
						wicones->first_selected = &wicones->entry[i];
						break;
					}

				WinCatalog_Refresh( win);
								
				if ( wicones->first_selected->txt_pos.y2 > h)
					while ( wicones->first_selected->txt_pos.y2 + wicones->case_h > h)
					{
						wicones->first_selected->txt_pos.y2 -= wicones->case_h;
						win->ypos++; 
					}  
	   
				draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);
				WindMakePreview( win);				

				break;	
	
	
			case SC_DEL:
				if ( wicones->first_selected)
				 	delete_entry( win);
				break;
	
/*			case SC_F1:
				{
					Entry *entry_ptr = wicones->first_selected;
					
					while( entry_ptr)
					{
						zdebug( "entry = %s", entry_ptr);
						entry_ptr = entry_ptr->next_selected;
					}
					zdebug( "---");
				}
				break;

				

			case SC_F3:
			{
				MFDB nicon, nicon_selected, icon_mask, screen = {0};
				int16	xy[8], fvdi = 1;

				if( create_icon( "c://1.gif", &nicon, &nicon_selected, &icon_mask))
				{
					int16	color[2], blitmode;

					xy[0] = 0;
					xy[1] = 0;
					xy[2] = nicon.fd_w - 1;
					xy[3] = nicon.fd_h - 1;

					xy[4] = 40;
					xy[5] = 40;
					xy[6] += nicon.fd_w - 1;
					xy[7] += nicon.fd_h - 1;

					vro_cpyfm( app.handle, S_ONLY, xy, &nicon, &screen);

					xy[4] += nicon.fd_w - 1;
					xy[6] += nicon.fd_h - 1;

					vro_cpyfm( app.handle, S_ONLY, xy, &nicon_selected, &screen);

					xy[4] += nicon.fd_w - 1;
					xy[6] += nicon.fd_h - 1;
		
					color[0] = WHITE;		
					color[1] = BLACK;

					vrt_cpyfm( app.handle, MD_REPLACE, xy, &icon_mask, &screen, color);


					xy[4] += nicon.fd_w - 1;
					xy[6] += nicon.fd_h - 1;

			        blitmode = app.nplanes > 8 ? S_AND_D : S_OR_D;

					if( fvdi)
					{
						color[0] = WHITE;		
						color[1] = BLACK;
						vrt_cpyfm( app.handle, 4, xy, &icon_mask, &screen, color);
					}
					else
					{
						color[0] = BLACK;		
						color[1] = WHITE;
						vrt_cpyfm( app.handle, MD_TRANS, xy, &icon_mask, &screen, color);
					}

					vro_cpyfm( app.handle, S_OR_D, xy, &nicon, &screen);
											
					delete_mfdb( &nicon);
					delete_mfdb( &nicon_selected);
					delete_mfdb( &icon_mask);

				}
				else
					zdebug("error");
			}
			break;	

			case SC_F4:
			{
				int16 mode = mode2logic( T_TRANSPARENT);

				zdebug( "%i", mode);			
			}						
	*/
			default:
				break;	
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
	
				if( old_entry->type == ET_IMAGE)	
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
			else if(( wicones->first_selected->type != old_entry->type) && ( wicones->first_selected->type == ET_IMAGE || old_entry->type == ET_IMAGE))		
				ObjcDraw( OC_TOOLBAR, win, TOOLBAR_SAVE, 1);		
		}	 	
	}
	else if ( wicones->edit)
	{
		Mini_Entry *mini_ptr = NULL;
		int16 c = evnt.keybd & 0x00FF;
		int16 max_len = ( int16)Dpathconf( wicones->directory, 3);

		switch( evnt.keybd >> 8) 
		{
			case SC_RETURN:
			case SC_ENTER:
				if ( wicones->first_selected->type == ET_DIR)
				{
					strcpy( fullname, wicones->directory);
					strcat( fullname, wicones->first_selected->name);
					strcat( fullname, "\\");
				}

				if( !rename_entry( win, wicones->edit->top->buf))
				{
					exit_edit_mode( win, wicones->first_selected);
					break;
				}

				strcpy( wicones->first_selected->name_shown, wicones->first_selected->name);

				if (( wicones->first_selected->icon_txt_w = get_text_width( wicones->first_selected->name_shown)) >= wicones->case_w - 4)
					wicones->first_selected->icon_txt_w = name_shorter( wicones->case_w - 6, wicones->first_selected->name_shown);

				exit_edit_mode( win, wicones->first_selected);

				// Now, we change the name of the dir in the file browser if necessary( left frame).
				if ( wicones->first_selected->type != ET_DIR)
					break;
				
				if (( mini_ptr = find_mini_entry_by_path( wicones, fullname)) != NULL)
				{
					strcpy( mini_ptr->foldername, mini_ptr->parent->foldername);		
					strcat( mini_ptr->foldername, wicones->first_selected->name);
					strcat( mini_ptr->foldername, "\\");
					strcpy( mini_ptr->name, wicones->first_selected->name);
					mini_ptr->icon_txt_w = get_text_width( mini_ptr->name);
					mini_ptr->icon_position.x2  = mini_ptr->icon_position.x1 + 15 + x_space + mini_ptr->icon_txt_w;

					if( browser_frame_width)
						redraw_mini_entry( win, mini_ptr);
				}
				
				break; 

			case SC_LFARW:
				if ( curs_left( wicones->edit))
					edit_icon_txt( win, wicones->first_selected);
				break;

			case SC_RTARW:
				if ( curs_right( wicones->edit))
					edit_icon_txt( win, wicones->first_selected);
				break; 

			case SC_UPARW:
			case SC_DWARW:
			case SC_ESC:
				exit_edit_mode( win, wicones->first_selected);
				break;

			case SC_DEL:
				curs_right( wicones->edit);
			case SC_BACK:			
				if ( char_del( wicones->edit) != 0) 
					edit_icon_txt( win, wicones->first_selected);
				break; 

			default:
				if (( wicones->edit->top->len < max_len ||  wicones->edit->top->len > 10000) && strchr( allowed_char, c)) 
				{
					char_put ( wicones->edit, c); 
					edit_icon_txt( win, wicones->first_selected);
				}
				break; 
		}
	}
}
