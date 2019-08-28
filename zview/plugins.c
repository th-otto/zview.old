#include "general.h"
#include "ztext.h"
#include "plugins.h"
#include "pic_load.h"
#include "plugins/common/zvplugin.h"
#include "plugins/common/plugver.h"



/* Global variable */
int16_t plugins_nbr;
int16_t encoder_plugins_nbr;
CODEC *codecs[MAX_CODECS];


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
static void plugin_free(CODEC *codec)
{
	switch (codec->type)
	{
	case CODEC_LDG:
		ldg_close(codec->c.ldg, ldg_global);
		break;
	case CODEC_SLB:
		plugin_close(&codec->c.slb);
		break;
	}
	free(codec->name);
	free(codec);
}

void plugins_quit( void)
{
	int16 i;

	for( i = 0; i < plugins_nbr; i++)
	{
		plugin_free(codecs[i]);
	}
	plugins_nbr = 0;
}


static boolean warn_duplicates(CODEC *last_codec, const char *ext, CODEC *this_codec)
{
	char buf[256];
	
	sprintf(buf, get_string(AL_DUPLICATE), ext, last_codec->name, this_codec->name);
	return FormAlert(1, buf);
}


static boolean check_duplicates(void)
{
	int16_t last = plugins_nbr;
	int16_t i;
	CODEC *last_codec = codecs[last];
	CODEC *this_codec;
	const char *p1;
	const char *p2;
	char ext1[4];
	char ext2[4];
	int16_t j, k;
	int ret;
	
	ext1[3] = '\0';
	ext2[3] = '\0';
	for (i = 0; i < last; i++)
	{
		this_codec = codecs[i];
		if (this_codec->num_extensions == 0)
		{
			if (last_codec->num_extensions == 0)
			{
				for (p1 = last_codec->extensions; *p1; p1 += strlen(p1) + 1)
				{
					for (p2 = this_codec->extensions; *p2; p2 += strlen(p2) + 1)
					{
						if (strcmp(p1, p2) == 0)
						{
							ret = warn_duplicates(last_codec, p1, this_codec);
							goto done;
						}
					}
				}
			} else
			{
				for (p1 = last_codec->extensions, j = 0; j < (int16_t)last_codec->num_extensions; p1 += 3, j++)
				{
					ext1[0] = p1[0];
					ext1[1] = p1[1];
					ext1[2] = p1[2];
					for (p2 = this_codec->extensions; *p2; p2 += strlen(p2) + 1)
					{
						if (strcmp(ext1, p2) == 0)
						{
							ret = warn_duplicates(last_codec, ext1, this_codec);
							goto done;
						}
					}
				}
			}
		} else
		{
			if (last_codec->num_extensions == 0)
			{
				for (p1 = last_codec->extensions; *p1; p1 += strlen(p1) + 1)
				{
					for (p2 = this_codec->extensions, j = 0; j < (int16_t)this_codec->num_extensions; p2 += 3, j++)
					{
						ext2[0] = p2[0];
						ext2[1] = p2[1];
						ext2[2] = p2[2];
						if (strcmp(p1, ext2) == 0)
						{
							ret = warn_duplicates(last_codec, p1, this_codec);
							goto done;
						}
					}
				}
			} else
			{
				for (p1 = last_codec->extensions, k = 0; k < (int16_t)last_codec->num_extensions; p1 += 3, k++)
				{
					ext1[0] = p1[0];
					ext1[1] = p1[1];
					ext1[2] = p1[2];
					for (p2 = this_codec->extensions, j = 0; j < (int16_t)this_codec->num_extensions; p2 += 3, j++)
					{
						ext2[0] = p2[0];
						ext2[1] = p2[1];
						ext2[2] = p2[2];
						if (strcmp(ext1, ext2) == 0)
						{
							ret = warn_duplicates(last_codec, ext1, this_codec);
							goto done;
						}
					}
				}
			}
		}
	}

	plugins_nbr++;
	return TRUE;

done:
	switch (ret)
	{
	case 1:
		/* replace previous by current */
		plugin_free(codecs[i]);
		codecs[i] = last_codec;
		return TRUE;
	case 2:
		/* ignore current */
		plugin_free(last_codec);
		return TRUE;
	}
	return FALSE;
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
	char plugin_dir[MAX_PATH];
	DIR	 			*dir;
	LDG_INFOS 		*cook = 0;
	struct dirent	*de;
	int16			len;
	char  			extension[4];
	char *name;
	CODEC *codec;
	int16_t i, j;
	
	strcpy( plugin_dir, zview_path);
	strcat( plugin_dir, "codecs\\");
	
	/* We try to find the zcodecs folder in zview directory... 	*/
	if ((dir = opendir(plugin_dir)) == NULL)
	{
		char 			*env_ldg = 0;

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

		dir = opendir(plugin_dir);
	}

	if (dir != NULL)
	{
		name = plugin_dir + strlen(plugin_dir);
		while ((de = readdir(dir)) != NULL && plugins_nbr < MAX_CODECS)
		{
			len = (int16)strlen(de->d_name);
			if (len < 3)
				continue;
			strcpy(name, de->d_name);
			strcpy(extension, de->d_name + len - 3);
			str2lower(extension);

			if (strcmp(extension, "ldg") == 0)
			{
				codec = codecs[plugins_nbr] = (CODEC *)calloc(1, sizeof(CODEC));
				if (codec == NULL)
				{
					errshow(NULL, -ENOMEM);
					break;
				}
				if ((codec->c.ldg = ldg_open(plugin_dir, ldg_global)) != NULL)
				{
					void CDECL(*codec_init)(void);

					if ((codec_init = ldg_find("plugin_init", codec->c.ldg)) != NULL)
					{
						codec->type = CODEC_LDG;
						codec->extensions = codec->c.ldg->infos;
						codec->num_extensions = codec->c.ldg->user_ext;
						codec->capabilities = 0;
						if (ldg_find("reader_init", codec->c.ldg) != 0)
							codec->capabilities |= CAN_DECODE;
						if (ldg_find("encoder_init", codec->c.ldg) != 0)
							codec->capabilities |= CAN_ENCODE;
						codec->name = strdup(de->d_name);
						if (!check_duplicates())
							break;
						codec_init();
					} else
					{
						errshow(de->d_name, LDG_ERR_BASE + ldg_error());
						plugin_free(codec);
					}
				} else
				{
					errshow(de->d_name, LDG_ERR_BASE + ldg_error());
				}
			} else if (strcmp(extension, "slb") == 0)
			{
				SLB *slb;
				long err;
				
				codec = codecs[plugins_nbr] = (CODEC *)calloc(1, sizeof(CODEC));
				if (codec == NULL)
				{
					errshow(NULL, -ENOMEM);
					break;
				}
				slb = &codec->c.slb;
				*name = '\0';
				if ((err = plugin_open(de->d_name, plugin_dir, slb)) >= 0)
				{
					codec->type = CODEC_SLB;
					codec->extensions = (const char *)plugin_get_option(slb, OPTION_EXTENSIONS);
					codec->num_extensions = 0;
					codec->capabilities = plugin_get_option(slb, OPTION_CAPABILITIES);
					if (codec->capabilities < 0)
						codec->capabilities = 0;
					codec->name = strdup(de->d_name);
					if (!check_duplicates())
						break;
				} else
				{
					errshow(de->d_name, err);
					free(codec);
				}
			}
		}
		closedir(dir);
	}

	/*
	 * now sort the encoders first, so we can use the same array when saving
	 */
	j = 0;
	for (i = 0; i < plugins_nbr; i++)
	{
		if (codecs[i]->capabilities & CAN_ENCODE)
		{
			CODEC *tmp = codecs[i];
			codecs[i] = codecs[j];
			codecs[j] = tmp;
			j++;
		}
	}
	encoder_plugins_nbr = j;

	return plugins_nbr;
}
