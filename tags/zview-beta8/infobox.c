#include "general.h"
#include "ztext.h"
#include "infobox.h"
#include "zedit/zedit.h"
#include "close_modal.h"
#include "pic_load.h"
#include "winimg.h"
#include "file/count.h"
#include "custom_font.h"
#include <math.h>

/* Prototype */
void generic_form_event( WINDOW *win);
void infobox( void);

static int16 dum, res;
static int16 posy, xy[8], dum;
static uint32 ypos_max, ypos;
float slider_pos = 0.0, slider_step = 0.0;
static	OBJECT	*infotext;
static txt_data *exif_box;
static OBJECT *slider_root;

static short vdi_intin[VDI_INTINMAX];
static short vdi_intout[VDI_INTOUTMAX];
static short vdi_ptsin[VDI_PTSINMAX];
static short vdi_ptsout[VDI_PTSOUTMAX];
static short vdi_control[VDI_CNTRLMAX];

static VDIPB vdi_params = {
        &vdi_control[0],
        &vdi_intin[0],
        &vdi_ptsin[0],
        &vdi_intout[0],
        &vdi_ptsout[0] 
};

static int my_vsl_color (short handle, short idx) {
	vdi_intin[0] = idx;

	vdi_control[0] = 17;
	vdi_control[1] = vdi_control[5] = 0;
	vdi_control[3] = 1;
	vdi_control[6] = handle;

	vdi (&vdi_params);

	return ((int)vdi_intout[0]);
}

void CDECL draw_exif_info( WINDOW *win, PARMBLK *pblk, void *data)
{
	exif_box = (txt_data *) data; 

	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = xy[0] + pblk->pb_w - 2;
	xy[3] = xy[1] + pblk->pb_h - 1;

	vsf_color( win->graf.handle, WHITE);
	v_bar( win->graf.handle, xy);

	xy[0] = pblk->pb_x + pblk->pb_w - 1;
	xy[1] = pblk->pb_y;
	xy[2] = pblk->pb_x;
	xy[3] = pblk->pb_y;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y + pblk->pb_h - 1;
	xy[6] = xy[0];
	xy[7] = xy[5];
	
	my_vsl_color( win->graf.handle, BLACK);
	v_pline( win->graf.handle, 4, xy);

	posy = pblk->pb_y;  

	for ( dum = ypos; dum < exif_box->lines; dum++, posy += 12) 
	{
		if( posy >= ( pblk->pb_y + pblk->pb_h - 1))
			break;

		draw_text( win->graf.handle, pblk->pb_x + 3, posy + 3, BLACK, exif_box->txt[dum]);
	}
}


void calc_exif_slider( WINDOW *win)
{
	int16 max_mover_size;
	int16 full_win_size = ypos_max * 12;

	max_mover_size 	= slider_root[FILE_INFO_VBACK].ob_height;

	slider_root[FILE_INFO_VMOVER].ob_y 		= 0;
	slider_root[FILE_INFO_VMOVER].ob_height = max_mover_size;
		
	if ( slider_root[FILE_INFO_COMMENT].ob_height < full_win_size)
	{	
		float factor 			= ( float)slider_root[FILE_INFO_COMMENT].ob_height / ( float)full_win_size;
		float mover_size 		= MAX(( float)max_mover_size * factor, 6L);
		
		slider_pos = 0;
		slider_step = ( float)max_mover_size / ( float)ypos_max;
		slider_root[FILE_INFO_VMOVER].ob_height	= ( int16)( mover_size);
	}	
}



static void slid_up( WINDOW *win, int obj_index, int mode) 
{
	ObjcChange( mode, win, obj_index, SELECTED, TRUE);
	
	do 
	{
		if ( ypos > 0L) 
		{
			ypos--;
			ObjcDraw( OC_FORM, win, FILE_INFO_COMMENT, 1);
			slider_pos -= slider_step;
			slider_root[FILE_INFO_VMOVER].ob_y	= MAX( 0, ( int16)floor( slider_pos));
			ObjcDraw( OC_FORM, win, FILE_INFO_VBACK, 2);
		}
		graf_mkstate( &dum, &dum, &res, &dum);

	} while( res);

	ObjcChange( mode, win, obj_index, ~SELECTED, TRUE);
}


static void slid_down( WINDOW *win, int obj_index, int mode) 
{
	ObjcChange( mode, win, obj_index, SELECTED, TRUE);

	do 
	{
		if (( ypos < ( ypos_max - slider_root[FILE_INFO_COMMENT].ob_height / 12)) && ( ypos_max  > slider_root[FILE_INFO_COMMENT].ob_height / 12)) 
		{
			ypos ++;
			ObjcDraw( OC_FORM, win, FILE_INFO_COMMENT, 1);
			slider_pos += slider_step;
			slider_root[FILE_INFO_VMOVER].ob_y	= ( int16)floor( slider_pos);
			ObjcDraw( OC_FORM, win, FILE_INFO_VBACK, 2);
		}
		graf_mkstate( &dum, &dum, &res, &dum);

	} while( res);

	ObjcChange( mode, win, obj_index, ~SELECTED, TRUE);
}


static void slid_vmover( WINDOW *win, int obj_index, int mode) 
{
	int32 old_ypos = ypos;

	ObjcChange( mode, win, obj_index, SELECTED, TRUE);

	graf_mouse( FLAT_HAND, NULL);

	while( !wind_update( BEG_MCTRL));

	res = graf_slidebox( slider_root, FILE_INFO_VBACK, FILE_INFO_VMOVER, 1);

	wind_update( END_MCTRL);

	ypos = MAX( 0, ( ypos_max - slider_root[FILE_INFO_COMMENT].ob_height / 12) * res / 1000L);

	if( old_ypos != ypos)
	{
		slider_pos = ypos * slider_step;
		slider_root[FILE_INFO_VMOVER].ob_y	= ( int16)floor( slider_pos);
		ObjcDraw( OC_FORM, win, FILE_INFO_VBACK, 2);	
		ObjcDraw( OC_FORM, win, FILE_INFO_COMMENT, 1);
	}

	graf_mouse( ARROW, NULL);

	ObjcChange( mode, win, obj_index, ~SELECTED, TRUE);
}


/*==================================================================================*
 * void infobox:																	*
 *		Show the infobox for a image or a file.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void infobox( void)
{
	WINDOW 			*wininfo;
	WINDICON 		*wicones;
    WINDATA			*windata;
    IMAGE 			*img;
	char 			temp[30];
	float 			uncompressed_size;
	int32			file_size;
	int16			name_len;
	int 			frms[] = { FILE_INFO_PANEL1, FILE_INFO_PANEL2, 	FILE_INFO_PANEL3};
	int 			buts[] = { FILE_INFO_FILE,   FILE_INFO_IMAGE,   FILE_INFO_EXIF};

	infotext = get_tree( FILE_INFO);

	if(( wicones = ( WINDICON *)DataSearch( wglb.front, WD_ICON)))	/* See if the target picture is in the catalog window */
	{		
		Entry *entry = wicones->first_selected;
		img 		 = &entry->preview;
		file_size 	 = entry->stat.st_size;

		if( wicones->edit)
			exit_edit_mode( win_catalog, wicones->first_selected);
		
		name_len = MIN( 29, strlen( entry->name));

		zstrncpy( temp, entry->name, name_len+1);

		ObjcStrCpy( infotext, FILE_INFO_NAME, temp);
		ObjcStrCpy( infotext, FILE_INFO_DATE, entry->date);
		ObjcStrCpy( infotext, FILE_INFO_TIME, entry->time);  

		if( entry->type == ET_DIR)
		{
			fileinfo file;
			
			graf_mouse( BUSYBEE, NULL);	

			if( !count_files( wicones, &file))
			{
				errshow( "", errno);
				return;
			}

			size_to_text( temp, ( float)file.size);
			ObjcStrCpy( infotext, FILE_INFO_SIZE, temp);

			graf_mouse( ARROW, NULL);
		}
		else
			ObjcStrCpy( infotext, FILE_INFO_SIZE, entry->size);  

	}
	else if(( windata = (WINDATA *)DataSearch( wglb.front, WD_DATA)))		/* See if the picture is in a "normal" window */
	{   
		struct stat		file_stat;
	   	struct tm		*tmt;
		int16			path_len, file_len;
		int8			*fullname = windata->name; 

		img 	 = &windata->img;
		file_len = ( int16)strlen( fullname);
		path_len = file_len;

		while(( fullname[path_len] != '/') && ( fullname[path_len] != '\\'))
			path_len--;

		name_len = MIN( 29, file_len - path_len);

		path_len++;
		
		zstrncpy( temp, &fullname[path_len], name_len+1);
		ObjcStrCpy( infotext, FILE_INFO_NAME, temp);

		lstat( fullname, &file_stat);
		file_size = file_stat.st_size;

		size_to_text( temp, ( float)file_stat.st_size);
		ObjcStrCpy( infotext, FILE_INFO_SIZE, temp);

		tmt = localtime(&file_stat.st_mtime);
		strftime( temp, 28, "%A %d %B %Y", tmt);
		ObjcStrCpy( infotext, FILE_INFO_DATE, temp);

		strftime( temp, 12, "%H:%M:%S", tmt);
		ObjcStrCpy( infotext, FILE_INFO_TIME, temp);
	}	  
	else 
		return;	/* normally, never */	


	if( img->comments)
		infotext[FILE_INFO_EXIF].ob_flags &= ~HIDETREE;
	else
		infotext[FILE_INFO_EXIF].ob_flags |= HIDETREE; 

	if ( img->page)
	{ 
		infotext[FILE_INFO_IMAGE].ob_flags &= ~HIDETREE;
	
		sprintf( infotext[FILE_INFO_COLOR].ob_spec.tedinfo->te_ptext,	"%ld", img->colors);	
		sprintf( infotext[FILE_INFO_WIDTH].ob_spec.tedinfo->te_ptext,  	"%d",  img->img_w);
		sprintf( infotext[FILE_INFO_HEIGHT].ob_spec.tedinfo->te_ptext, 	"%d",  img->img_h);	
		sprintf( infotext[FILE_INFO_PLANES].ob_spec.tedinfo->te_ptext,  "%d",  img->bits);	
		sprintf( infotext[FILE_INFO_PAGE].ob_spec.tedinfo->te_ptext,  	"%d",  img->page);
		ObjcStrCpy( infotext, FILE_INFO_DEC_TIME, img->working_time);
		ObjcStrCpy( infotext, FILE_INFO_COMPRESSION, img->compression);
		ObjcStrCpy( infotext, FILE_INFO_INFO, img->info);
		
		/* Calculate the Image decompressed size */
		uncompressed_size = (((( float)img->img_w / 8.0) + 1) * ( float)img->bits) * ( float)img->img_h;	
		size_to_text( infotext[FILE_INFO_MEM].ob_spec.tedinfo->te_ptext, uncompressed_size);
			
		sprintf( infotext[FILE_INFO_RATIO].ob_spec.tedinfo->te_ptext, "%.1f : 1", uncompressed_size / ( float)file_size);				
	}
	else
		infotext[FILE_INFO_IMAGE].ob_flags |= HIDETREE; 
		
	wininfo = FormCreate( infotext, NAME|MOVER|CLOSER, NULL, "Information", NULL, TRUE, FALSE);

	FormThumb( wininfo, frms, buts, 3);

	WindSet( wininfo, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	EvntAttach( wininfo, WM_CLOSED, close_modal);

	if( img->comments)
	{
		ypos 			= 0;
		ypos_max 		= img->comments->lines;
		wininfo->h_u 	= 12;
		
		slider_root 	= FORM( wininfo);
		
		calc_exif_slider( wininfo);

		RsrcUserDraw ( OC_FORM, wininfo, FILE_INFO_COMMENT, draw_exif_info, ( void*)img->comments);

		// draw_exif_info( wininfo);

		ObjcAttach( OC_FORM, wininfo, FILE_INFO_UP, BIND_FUNC, slid_up);
		ObjcAttach( OC_FORM, wininfo, FILE_INFO_DOWN, BIND_FUNC, slid_down);
		ObjcAttach( OC_FORM, wininfo, FILE_INFO_VMOVER, BIND_FUNC, slid_vmover);
	}


	MenuDisable();
}

