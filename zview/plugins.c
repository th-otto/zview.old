#include "general.h"
#include "ztext.h"

/* Prototype */
int16 plugins_init( void);
void plugins_quit( void);


/* LDG function */
void CDECL( *codec_init)( void) = NULL;

/* Global variable */
int16 	plugins_nbr = 0;
LDG 	*codecs[100];


/*==================================================================================*
 * void plugins_quit:																*
 *		unload all the codec from memory.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void plugins_quit( void)
{
	int16 i;

	for( i = 0; i < plugins_nbr; i++)
		ldg_close( codecs[i], ldg_global);

}


/*==================================================================================*
 * void plugins_init:																*
 *		load all the codec to memory.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
int16 plugins_init( void)
{
	char 			*env_ldg, plugin_dir[MAX_PATH];
	DIR	 			*dir;
	LDG_INFOS 		*cook;
	struct dirent	*de;
	int16			len;
	char  			extention[4];

	strcpy( plugin_dir, zview_path);
	strcat( plugin_dir, "\\codecs");

	/* We try to find the zcodecs folder in zview directory... 	*/
	if ( chdir( plugin_dir) != 0)
	{
		shel_envrn( &env_ldg, "LDGPATH=");

		/* ...if this directory doesn't exist, we try to find 
	   	   the zcodecs folder in the default LDG path, with 
	   	   the LDG cookie or with the environment variable.	*/

		if( ldg_cookie( LDG_COOKIE, ( int32*)&cook))	
			strcpy( plugin_dir, cook->path);
		else if( env_ldg)
			strcpy( plugin_dir, env_ldg);
		else 
			strcpy( plugin_dir, "C:\\gemsys\\ldg\\");

		len = ( int16)strlen( plugin_dir);
		
		if( plugin_dir[len-1] != '\\')
			strcat( plugin_dir, "\\"); 

		strcat( plugin_dir, "codecs");

		if ( chdir( plugin_dir) != 0)
			return( 0);
	}

	if (( dir = opendir( ".")) != NULL)
	{
		while(( de = readdir( dir)) != NULL)
		{
			if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
				continue;

			strcpy ( extention, de->d_name + strlen( de->d_name) - 3);
			str2lower( extention);

			if( strcmp ( extention, "ldg") == 0)
			{
				if ( ( codecs[plugins_nbr] = ldg_open( de->d_name, ldg_global)))
				{
					if ( ( codec_init = ldg_find( "plugin_init", codecs[plugins_nbr])))
					{
						codec_init();
						plugins_nbr++;
					}
					else
					{
						errshow( de->d_name, ldg_error());
						ldg_close( codecs[plugins_nbr], ldg_global);
					}
				}
				else
				{
					errshow( de->d_name, ldg_error());
				}
			}
		}
		closedir( dir);
	}

	chdir( zview_path);

	return plugins_nbr;
}

