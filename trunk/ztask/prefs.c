#include "general.h"

int		button_off_background			= 8;
int		button_off_light_color			= 250;
int		button_off_dark_color			= 252;
int		button_off_text_color			= 255;
int		button_off_text_shadow_color	= 250;
int		button_on_background			= 252;
int		button_on_light_color			= 8;
int		button_on_dark_color 			= 144;
int		button_on_text_color			= 1;
int		button_on_text_shadow_color		= 251;
int		geek_area_text_color			= 1;
int		geek_area_color 				= 252;
int		geek_area_dark_line				= 144;
int		geek_area_light_line			= 250;
int		app_width						= 72;
int		cpu_bar_color 					= 146;
int		tt_bar_color 					= 73;
int		st_bar_color 					= 106;
int		show_clock 						= TRUE;
int		clock_us						= FALSE;
int		show_system_info				= TRUE;
int		show_acc						= TRUE;

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

		strcat( filename, "ztask.inf");

		if (( inffile = fopen( filename, "rb+")) != NULL)
			goto loop;

	}

	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( "ztask.inf", "rb+")) == NULL)
	{
		if( app.nplanes < 8)
		{
			button_off_light_color			= WHITE;
			button_off_dark_color			= LBLACK;
			button_off_text_color			= BLACK;
			button_off_text_shadow_color	= WHITE;
			button_on_background			= LBLACK;
			button_on_light_color			= LWHITE;
			button_on_dark_color 			= BLACK;
			button_on_text_color			= BLACK;
			button_on_text_shadow_color		= LWHITE;
			geek_area_color 				= LWHITE;
			geek_area_dark_line				= LBLACK;
			geek_area_light_line			= WHITE;
			cpu_bar_color 					= CYAN;
			tt_bar_color 					= BLUE;
			st_bar_color 					= RED;			
		}
		
		return( 0);
	}

loop:
		
	do 
	{
		valid_entry_found = FALSE;
			
		filepos = ftell( inffile); 

		if ( fscanf( inffile, "button_off_background=%d ", &button_off_background) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if (fscanf( inffile, "button_off_light_color=%d ", &button_off_light_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
	
		if ( fscanf( inffile, "button_off_dark_color=%d ", &button_off_dark_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);	

		if (fscanf( inffile, "button_off_text_color=%d ", &button_off_text_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
	
		if ( fscanf( inffile, "button_off_text_shadow_color=%d ", &button_off_text_shadow_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);				
												
		if ( fscanf( inffile, "button_on_background=%d ", &button_on_background) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			

		if (fscanf( inffile, "button_on_light_color=%d ", &button_on_light_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "button_on_dark_color=%d ", &button_on_dark_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if (fscanf( inffile, "button_on_text_color=%d ", &button_on_text_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
	
		if ( fscanf( inffile, "button_on_text_shadow_color=%d ", &button_on_text_shadow_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);				
			
		if ( fscanf( inffile, "geek_area_text_color=%d ", &geek_area_text_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
			
			
		if ( fscanf( inffile, "geek_area_color=%d ", &geek_area_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

			
		if ( fscanf( inffile, "geek_area_dark_line=%d ", &geek_area_dark_line) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "geek_area_light_line=%d ", &geek_area_light_line) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);			
			
		if ( fscanf( inffile, "app_width=%d ", &app_width) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

							
		if ( fscanf( inffile, "cpu_bar_color=%d ", &cpu_bar_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);


		if ( fscanf( inffile, "tt_bar_color=%d ", &tt_bar_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "st_bar_color=%d ", &st_bar_color) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_clock=%d ", &show_clock) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "clock_us=%d ", &clock_us) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_system_info=%d ", &show_system_info) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "show_acc=%d ", &show_acc) == 1)
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
	char 	filename[256] = "ztask.inf";
	FILE 	*inffile;
	char 	*env_home;
	int16  	len;	

	shel_envrn( &env_home, "HOME=");

	/* Home directory exist? */
	if ( env_home != NULL)
	{
		strcpy( filename, env_home);

		len = ( int16)strlen( filename);

		if ((filename[len - 1] != '\\') && (filename[len - 1] != '/'))
			strcat( filename, "\\");

		strcat( filename, "ztask.inf");
	}

	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( filename, "wb+")) == NULL)
		return( 0);

	fprintf( inffile, "button_off_background=%d\r\n", button_off_background);
	fprintf( inffile, "button_off_light_color=%d\r\n", button_off_light_color);	
	fprintf( inffile, "button_off_dark_color=%d\r\n", button_off_dark_color);	
	fprintf( inffile, "button_off_text_color=%d\r\n", button_off_text_color);	
	fprintf( inffile, "button_off_text_shadow_color=%d\r\n", button_off_text_shadow_color);		
	fprintf( inffile, "button_on_background=%d\r\n", button_on_background);	
	fprintf( inffile, "button_on_light_color=%d\r\n", button_on_light_color);
	fprintf( inffile, "button_on_dark_color=%d\r\n", button_on_dark_color);
	fprintf( inffile, "button_on_text_color=%d\r\n", button_on_text_color);	
	fprintf( inffile, "button_on_text_shadow_color=%d\r\n", button_on_text_shadow_color);	
	fprintf( inffile, "geek_area_text_color=%d\r\n", geek_area_text_color);
	fprintf( inffile, "geek_area_color=%d\r\n", geek_area_color);
	fprintf( inffile, "geek_area_dark_line=%d\r\n", geek_area_dark_line);
	fprintf( inffile, "geek_area_light_line=%d\r\n", geek_area_light_line);	
	fprintf( inffile, "app_width=%d\r\n", app_width);
	fprintf( inffile, "cpu_bar_color=%d\r\n", cpu_bar_color);
	fprintf( inffile, "tt_bar_color=%d\r\n", tt_bar_color);
	fprintf( inffile, "st_bar_color=%d\r\n", st_bar_color);
	fprintf( inffile, "show_clock=%d\r\n", show_clock);
	fprintf( inffile, "clock_us=%d\r\n", clock_us);
	fprintf( inffile, "show_system_info=%d\r\n", show_system_info);
	fprintf( inffile, "show_acc=%d\r\n", show_acc);
	fclose( inffile);

	return( 1);
}
