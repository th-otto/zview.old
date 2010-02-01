#include "general.h"

/* thumbnail size in a 3D integer*/
int thumbnail[8][2] = { /* 4:3 size */ { 64, 48}, { 96, 72}, { 120, 90}, { 180, 135}, 
						/* 3:4 size */ { 48, 64}, { 72, 96}, { 90, 120}, { 135, 180}};

/* File's Browser Preferences */
int show_size					= 1;			/* show file size 											*/
int show_hidden					= 0;			/* show hidden file 										*/
int show_non_image				= 1;			/* show non-image file 										*/
int browser_x					= 0;			/* Win catalog x, y, w, h position/size						*/
int browser_y					= 0;			/* "														*/
int browser_w					= 0;			/* "														*/
int browser_h					= 0;			/* "														*/
int sortingmode					= 0;			/* File's sorting mode: 0) by name, 1) by size, 2) by date	*/
int thumbnail_size				= 1;			/* thumbnail size 											*/
int browser_frame_width			= 200;			/* folder's browser frame width								*/
int preview_frame_height		= 200;			/* as the integer's name say :)								*/
int show_read_progress_bar		= 1;			/* show the read progress bar								*/
int show_write_progress_bar		= 1;			/* show the write progress bar								*/
char skip_drive[27] 			= "ABU";		/* Skip this drives in the browser							*/
int smooth_thumbnail 			= 0;			/* 0 = None, 1 = Triangle, 2 = Blackman, 3 = Gaussian, 4 = Quadratic, 5 = Cubic, 6 = Lanczos */
int pdf_fit_to_win 				= 1;			/* Fit the PDF to the Window								*/
int pdf_aa		 				= 1;			/* Font Antialiasing										*/

/* Prototype */
int prefs_read( void);
int prefs_write( void);


/*==================================================================================*
 * prefs_read:																		*
 *	Reads the content of the INF file an put it in the preference global variables.	*
 *----------------------------------------------------------------------------------*
 * returns: 	0 if error															*
 *==================================================================================*/
int prefs_read( void)
{
	char filename[256] = "";
	char unknown_s[256] = "";
	FILE *inffile;
	int32 filepos;
	char *env_home;
	int16  len;	
	boolean valid_entry_found;

	
	shel_envrn( &env_home, "HOME=");

	/* Home directory exist? */
	if ( env_home != NULL)
	{
		strcpy( filename, env_home);
		len = ( int16)strlen( filename);

		if ((filename[len - 1] != '\\') && (filename[len - 1] != '/'))
		{
			strcat( filename, "\\");
		}

		strcat( filename, "zview.inf");

		if (( inffile = fopen( filename, "rb+")) != NULL)
			goto loop;

	}

	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( "zview.inf", "rb+")) == NULL)
		return( 0);

loop:
		
	do 
	{
		valid_entry_found = FALSE;
			
		filepos = ftell( inffile); 

		if ( fscanf( inffile, "show_size=%d ", &show_size) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);
	
		if ( fscanf( inffile, "show_hidden=%d ", &show_hidden) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_non_image=%d ", &show_non_image) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "browser_x=%d ", &browser_x) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "browser_y=%d ", &browser_y) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);
							
							
		if ( fscanf( inffile, "browser_w=%d ", &browser_w) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "browser_h=%d ", &browser_h) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "sortingmode=%d ", (int *)&sortingmode) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "thumbnail_size=%d ", &thumbnail_size) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "browser_frame_width=%d ", &browser_frame_width) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "preview_frame_height=%d ", &preview_frame_height) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_read_progress_bar=%d ", &show_read_progress_bar) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_write_progress_bar=%d ", &show_write_progress_bar) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "skip_drive=%s ", skip_drive) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);
			
		if ( fscanf( inffile, "smooth_thumbnail=%d ", &smooth_thumbnail) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
			
		if ( fscanf( inffile, "pdf_fit_to_win=%d ", &pdf_fit_to_win) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);	

		if ( fscanf( inffile, "pdf_aa=%d ", &pdf_aa) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);				
						
		if ( fscanf( inffile, "%s ", unknown_s) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

	} while ( valid_entry_found); 

	fclose( inffile);

	return( 1);
}



/*==================================================================================*
 * prefs_write:																		*
 *	Writes the preference variables in the preference file.							*
 *----------------------------------------------------------------------------------*
 * returns: 	0 if error															*
 *==================================================================================*/
int prefs_write( void)
{
	char 	filename[256] = "";
	FILE 	*inffile;
	char 	*env_home;
	int16  	len;	

	shel_envrn( &env_home, "HOME=");

	/* Home directory exist? */
	if ( env_home != NULL)
		strcpy( filename, env_home);
	else
		strcpy( filename, zview_path);

	len = ( int16)strlen( filename);

	if ((filename[len - 1] != '\\') && (filename[len - 1] != '/'))
		strcat( filename, "\\");

	strcat( filename, "zview.inf");

	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( filename, "wb+")) == NULL)
		return( 0);

	fprintf( inffile, "show_size=%d\r\n", show_size);
	fprintf( inffile, "show_hidden=%d\r\n", show_hidden);
	fprintf( inffile, "show_non_image=%d\r\n", show_non_image);
	fprintf( inffile, "browser_x=%d\r\n", browser_x);
	fprintf( inffile, "browser_y=%d\r\n", browser_y);
	fprintf( inffile, "browser_w=%d\r\n", browser_w);
	fprintf( inffile, "browser_h=%d\r\n", browser_h);
	fprintf( inffile, "sortingmode=%d\r\n", (int )sortingmode);
	fprintf( inffile, "thumbnail_size=%d\r\n", thumbnail_size);
	fprintf( inffile, "browser_frame_width=%d\r\n", browser_frame_width);
	fprintf( inffile, "preview_frame_height=%d\r\n", preview_frame_height);
	fprintf( inffile, "show_read_progress_bar=%d\r\n", show_read_progress_bar);
	fprintf( inffile, "show_write_progress_bar=%d\r\n", show_write_progress_bar);
	fprintf( inffile, "skip_drive=%s\r\n", skip_drive);
	fprintf( inffile, "smooth_thumbnail=%d\r\n", smooth_thumbnail);	
	fprintf( inffile, "pdf_fit_to_win=%d\r\n", pdf_fit_to_win); 
	fprintf( inffile, "pdf_aa=%d\r\n", pdf_aa);
			
	fclose( inffile);
	return( 1);
}
