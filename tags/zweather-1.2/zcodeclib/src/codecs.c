#include "general.h"
#include "ztext.h"
#ifdef __PUREC__
#include "zvdi\vdi.h"
#else
#include "zvdi/vdi.h"
#endif


/* Prototype */
int16 plugins_init( void);
void plugins_quit( void);

/* LDG function */
void CDECL( *codec_init)( void) = NULL;

static int16 vdi_initialised = 0;
static int16 mem_initialised = 0;

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
void codecs_quit( void)
{
	int16 i;
	
	for( i = 0; i < plugins_nbr; i++)
	{
		ldg_close( codecs[i], ldg_global);
	}

	mem_quit();
}


/*==================================================================================*
 * void plugins_init:																*
 *		load codec(s) to memory.													*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		codec_name:	the name of the codec to load, if this one is "all", we load	* 
 *		the codec.																	*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      0 if error.																	*
 *==================================================================================*/
int16 codecs_init( char *codec_name)
{
	char 			*env_ldg, current_dir[1024], plugin_dir[1024];
	DIR	 			*dir;
	LDG_INFOS 		*cook;
	struct dirent	*de;
	int16			len;
	char  			extention[4];

	shel_envrn( &env_ldg, "LDGPATH=");

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

	current_dir[0] = 'A' + Dgetdrv();
	current_dir[1] = ':';
	Dgetpath( current_dir + 2, 0);

	if( vdi_initialised == 0)
		if( !( vdi_initialised = vdi_init()))
			return( 0);

	if( mem_initialised == 0)
		if( !( mem_initialised = mem_init()))
			return( 0);

	if ( chdir( plugin_dir) != 0)
	{
		mem_quit();
		return( 0);
	}

	if( strcmp ( codec_name, "all") == 0) 
	{
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
							ldg_close( codecs[plugins_nbr], ldg_global);
					}
				}
			}
			closedir( dir);
		}
	}
	else if ( ( codecs[plugins_nbr] = ldg_open( codec_name, ldg_global)))
	{
		if ( ( codec_init = ldg_find( "plugin_init", codecs[plugins_nbr])))
		{
			codec_init();
			plugins_nbr++;
		}
		else
			ldg_close( codecs[plugins_nbr], ldg_global);
	}

	chdir( current_dir);

	return plugins_nbr;
}

