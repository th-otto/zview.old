#include "general.h"
#include "ztext.h"
#include "plugins.h"
#include "pic_load.h"
#include "plugins/common/zvplugin.h"
#include "plugins/common/plugin_version.h"



/* Global variable */
int16 	plugins_nbr = 0;
CODEC codecs[MAX_CODECS];


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
	{
		switch (codecs[i].type)
		{
		case CODEC_LDG:
			ldg_close(codecs[i].c.ldg, ldg_global);
			break;
		case CODEC_SLB:
			plugin_close(&codecs[i].c.slb);
			break;
		}
 		codecs[i].type = CODEC_NONE;
	}
	plugins_nbr = 0;
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
	char 			*env_ldg = 0;
	char plugin_dir[MAX_PATH];
	DIR	 			*dir;
	LDG_INFOS 		*cook = 0;
	struct dirent	*de;
	int16			len;
	char  			extension[4];
	char *name;
	
	strcpy( plugin_dir, zview_path);
	strcat( plugin_dir, "codecs\\");
	name = plugin_dir + strlen(plugin_dir);
	
	/* We try to find the zcodecs folder in zview directory... 	*/
	if ((dir = opendir(plugin_dir)) == NULL)
	{
		shel_envrn( &env_ldg, "LDGPATH=");

		/* ...if this directory doesn't exist, we try to find 
	   	   the zcodecs folder in the default LDG path, with 
	   	   the LDG cookie or with the environment variable.	*/

		if( ldg_cookie( LDG_COOKIE, ( int32*)&cook) && cook)
			strcpy(plugin_dir, cook->path);
		else if( env_ldg)
			strcpy( plugin_dir, env_ldg);
		else 
			strcpy( plugin_dir, "C:\\gemsys\\ldg\\");

		len = ( int16)strlen( plugin_dir);
		
		if( len > 0 && plugin_dir[len-1] != '\\' && plugin_dir[len-1] != '/')
			strcat( plugin_dir, "\\"); 

		strcat( plugin_dir, "codecs\\");
		name = plugin_dir + strlen(plugin_dir);

		dir = opendir(plugin_dir);
	}

	if (dir != NULL)
	{
		while ((de = readdir(dir)) != NULL && plugins_nbr < MAX_CODECS)
		{
			len = (int16)strlen(de->d_name);
			if (len < 3)
				continue;
			strcpy(name, de->d_name);
			str2lower(de->d_name);
			if (strcmp(de->d_name, "xpdf.slb") == 0)
				continue;
			strcpy(extension, de->d_name + len - 3);

			if (strcmp(extension, "ldg") == 0)
			{
				if ((codecs[plugins_nbr].c.ldg = ldg_open(plugin_dir, ldg_global)) != NULL)
				{
					void CDECL(*codec_init)(void);

					if ((codec_init = ldg_find("plugin_init", codecs[plugins_nbr].c.ldg)) != NULL)
					{
						codecs[plugins_nbr].type = CODEC_LDG;
						codecs[plugins_nbr].extensions = codecs[plugins_nbr].c.ldg->infos;
						codecs[plugins_nbr].num_extensions = codecs[plugins_nbr].c.ldg->user_ext;
						codec_init();
						plugins_nbr++;
					} else
					{
						errshow(de->d_name, LDG_ERR_BASE + ldg_error());
						ldg_close( codecs[plugins_nbr].c.ldg, ldg_global);
					}
				} else
				{
					errshow(de->d_name, LDG_ERR_BASE + ldg_error());
				}
			} else if (strcmp(extension, "slb") == 0)
			{
				SLB *slb = &codecs[plugins_nbr].c.slb;
				long err;
				
				*name = '\0';
				if ((err = plugin_open(de->d_name, plugin_dir, slb)) >= 0)
				{
					codecs[plugins_nbr].type = CODEC_SLB;
					codecs[plugins_nbr].extensions = (const char *)plugin_get_option(slb, OPTION_EXTENSIONS);
					codecs[plugins_nbr].num_extensions = 0;
					plugins_nbr++;
				} else
				{
					errshow(de->d_name, err);
				}
			}
		}
		closedir(dir);
	}

	return plugins_nbr;
}
