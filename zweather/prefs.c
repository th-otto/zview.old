#include "general.h"

int		unit 				= WEATHER_METRIC;
int		iconify_at_start	= 1;
char	location_code[9]	= "BEXX0006";
clock_t	update_time 		= 360000L;

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
	int time_minute;
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

		strcat( filename, "zweather.inf");

		if (( inffile = fopen( filename, "rb+")) != NULL)
			goto loop;

	}

	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( "zweather.inf", "rb+")) == NULL)
		return( 0);

loop:

	do
	{
		valid_entry_found = FALSE;

		filepos = ftell( inffile);

		if ( fscanf( inffile, "unit=%d ", &unit) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if (fscanf( inffile, "iconify_at_start=%d ", &iconify_at_start) == 1)
		{
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "update_time=%d ", &time_minute) == 1)
		{
			update_time = ( clock_t)( ( int32)time_minute * 12000L);
			valid_entry_found = TRUE;
			continue;
		}
		else
			fseek( inffile, filepos, SEEK_SET);

		if ( fscanf( inffile, "location_code=%s ", location_code) == 1)
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
	int 	time_minute;
	int16  	len;

	shel_envrn( &env_home, "HOME=");

	/* Home directory exist? */
	if ( env_home)
	{
		strcpy( filename, env_home);

		len = ( int16)strlen( filename);

		if ((filename[len - 1] != '\\') && (filename[len - 1] != '/'))
			strcat( filename, "\\");

		strcat( filename, "zweather.inf");
	}
	else
		strcpy( filename, "zweather.inf");


	/* With ftell, we MUST open the file in binary mode and not in text mode !!! */
	if (( inffile = fopen( filename, "wb+")) == NULL)
		return( 0);


	time_minute = ( int)( update_time / 12000L);

	fprintf( inffile, "unit=%d\r\n", unit);
	fprintf( inffile, "iconify_at_start=%d\r\n", iconify_at_start);
	fprintf( inffile, "update_time=%d\r\n", time_minute);
	fprintf( inffile, "location_code=%s\r\n", location_code);
	fclose( inffile);

	return( 1);
}
