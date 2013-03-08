#include "../general.h"
#include "file.h"

/* Prototype */
int16 count_files( WINDICON *wicones, fileinfo *file);
int16 count_dir( const char *path, fileinfo *file);

/*==================================================================================*
 * int16 count_dir	:																*
 *		Do a recursive file count in a directory.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		path	-> the path where to search.										*
 *		item	-> the adress where to return the number of files.					*
 *		size	-> the adress where to return the total size of all the files.		* 
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      1 is ok else 0																*
 *==================================================================================*/

int16 count_dir( const char *path, fileinfo *file)
{
	char 			old_dir[MAX_PATH];
	DIR	 			*dir;
	struct dirent	*de;
	struct stat		file_stat;
	
	if ( dir_cd( path, old_dir, ( int16)( sizeof old_dir)))
	{
		if (( dir = opendir( ".")) != NULL)
		{
			while(( de = readdir( dir)) != NULL)
			{
				if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
					continue;	

				if ( lstat( de->d_name, &file_stat) != 0)
				{
					closedir( dir);
					dir_cd( old_dir, NULL, 0);
					return 0;
				}	
					
				if ( S_ISDIR( file_stat.st_mode))   
				{	
					if ( !count_dir( de->d_name, file))
					{
						closedir( dir);
						dir_cd( old_dir, NULL, 0);
						return 0;
					}
					else
						file->item ++;
				}
				else
				{
					file->item ++;		
					file->size += file_stat.st_size;
				}
			}
			closedir( dir);
		}
		else
			return 0;
		
		dir_cd( old_dir, NULL, 0);
	}
	else
		return 0;
	
	return 1;
}


/*==================================================================================*
 * int16 count_files:																*
 *		Do a recursive file count.													*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		wicones	-> the target wicones.												*
 *		item	-> the adress where to return the number of files.					*
 *		size	-> the adress where to return the total size of all the files.		* 
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      1 is ok else 0																*
 *==================================================================================*/

int16 count_files( WINDICON *wicones, fileinfo *file)
{
	Entry 	*entry 	= wicones->first_selected;

	file->item = 0L;		
	file->size = 0L;

	while ( entry)
	{
		if ( S_ISDIR( entry->stat.st_mode))
		{
			if( !count_dir( entry->name, file))
				return 0;
			else
				file->item ++;
		}	   
		else
		{
			file->item ++;		
			file->size += entry->stat.st_size;
		}

		entry = entry->next_selected;
	}

	return 1;
}
