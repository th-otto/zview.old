/*----------------------------------------------------------------------*/
/*						Files utilities									*/
/*----------------------------------------------------------------------*/
#include "../general.h"
#include "../zedit/zedit.h"
#include "../catalog/catalog_entry.h"
#include "../ztext.h"
#include "../catalog/catalog_icons.h"
#include "../prefs.h"
#include "../mfdb.h"
#include "../txt_data.h"
#include "../custom_font.h"
#include "sort.h"
#include "file.h"

#define ALLOC_NDIRENTRIES (4)				/* number of entries that should be allocated at once by readdir() */



/*==================================================================================*
 * dir_parent:																		*
 *		go to the parent directory. 												*
 *----------------------------------------------------------------------------------*
 *	returns: TRUE = success, FALSE = if root path ( so, no dir up).					*
 *----------------------------------------------------------------------------------*
 * 	dir:		Pointer to the current directory, the new directory will be 	 	* 
 *				returned on the same pointer.										*		
 * 	old_dir:	Pointer on a null string where will be returned the name of the 	*
 *				old directory. ( not the full path, only the name)					*		
 *==================================================================================*/

boolean dir_parent( char *dir, char *old_dir)
{
	int16 len, i, ii;

	len = ( int16) strlen( dir) - 1;
				
	if ( len < 3)
	   return FALSE;              

	if ( dir[len] == '\\')
		dir[len] = '\0';
			
	len--;
	i = len;

	while ( dir[i] != '\\')
		i--;

	ii = len - i - 1;

	old_dir[ii + 1] = '\0';
										
	while ( dir[len] != '\\')
	{
		old_dir[ii] = dir[len];
		dir[len] = '\0';
		len--;
		ii--;
	}

	return TRUE;	
}

/*==================================================================================*
 * dir_cd:																			*
 *		enter to a directory.											 			*
 *----------------------------------------------------------------------------------*
 * 	returns: TRUE = success, FALSE = error											*
 *----------------------------------------------------------------------------------*
 * 	new_dir:	The New directory. 	 												*
 * 	old_dir:	Pointer where save the previous directory. 	 						*
 * 				( If NULL, no storing is done).			 	 						*
 *==================================================================================*/
 
boolean dir_cd( const char *new_dir, char *old_dir, int16 bufflen)
{
	if ( old_dir != NULL)
		getcwd( old_dir, bufflen);
	
	return chdir( new_dir) == 0 ? TRUE : FALSE;
}


/*==================================================================================*
 * scan_dir:																		*
 *		scan the 'dirpath' directory and fit somes entries in 'dirfile'.			*
 *----------------------------------------------------------------------------------*
 * return: TRUE if ok or FALSE if error												*
 *==================================================================================*/

boolean scan_dir( WINDOW *win, const char *dirpath)
{
	short 			dum, allocated_entries = 0;
	Entry 			*entry;
	DIR	 			*dir;
	struct dirent	*de;
	struct stat		file_stat;
	struct tm		*tmt;
    WINDICON 		*dirfile = ( WINDICON *)DataSearch( win, WD_ICON);

	if ( dirfile->nbr_icons)
	{
		for ( dum = 0; dum < dirfile->nbr_icons; dum++)
		{
			delete_txt_data( &dirfile->entry[dum].preview);
			delete_mfdb( dirfile->entry[dum].preview.image, dirfile->entry[dum].preview.page);		
		}

		dirfile->nbr_icons = 0;
		
		free( dirfile->entry);
	}

	dirfile->first_selected = NULL;
	dirfile->entry = NULL;
	dirfile->edit = NULL;

	if ( !dir_cd( dirpath, NULL, 0))
		dir_cd( ".", NULL, 0);

	strcpy( dirfile->directory, dirpath);

	dum = ( int16)strlen( dirfile->directory);

	if ( dirfile->directory[dum-1] != '\\')
		strcat( dirfile->directory, "\\");
		
	if (( dir = opendir( ".")) == NULL)
	{
		errshow(".", -errno);
		goto abort;
	}

	while(( de = readdir( dir)) != NULL)
	{
		if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
			continue;	
		
		if ( stat( de->d_name, &file_stat) != 0)
			continue;

		if (((( file_stat.st_flags & FA_HIDDEN) != 0) || de->d_name[0] == '.') && !show_hidden)
			continue;

		/* allocate new memory for entries if needed */
		if ( dirfile->nbr_icons >= allocated_entries)
		{ /* allocate new entries */
			if ( dirfile->nbr_icons == 0)
			{ /* alloc for the first time */
				if (( dirfile->entry = ( Entry *)malloc( ALLOC_NDIRENTRIES * sizeof( Entry))) == NULL)
				{
					errshow(NULL, -ENOMEM);
					goto abort;
				}
			}
			else
			{ /* expand allocated memory */
				if (( entry = ( Entry *)realloc( dirfile->entry, ( allocated_entries + ALLOC_NDIRENTRIES) * sizeof( Entry))) == NULL)
				{
					errshow(NULL, -ENOMEM);
					goto abort;
				}
				dirfile->entry = entry;
			}
			allocated_entries += ALLOC_NDIRENTRIES;
		}
		/* fill out and add new entry */
		entry = &dirfile->entry[dirfile->nbr_icons];
		entry->preview.image		   	= NULL;
		entry->preview.page			   	= 0;
		entry->preview.comments 		= NULL;
		entry->next_selected 		   	= NULL;
		entry->stat 				   	= file_stat;
		strcpy( entry->name, de->d_name);
		zstrncpy( entry->name_shown, de->d_name, sizeof(entry->name_shown));

		/* Get Size */
		size_to_text( entry->size, ( float)file_stat.st_size);
 		/* Get Time */
		tmt = localtime(&file_stat.st_mtime);
		strftime( entry->date, 28, "%A %d %B %Y", tmt);
		strftime( entry->time, 12, "%H:%M:%S", tmt);

		set_entry_icon( entry);

		/* find if the entry name is bigger than the case width */	
		entry->icon_txt_w = name_shorter( dirfile->case_w - 4, entry->name_shown);

		dirfile->nbr_icons++;
	}

	closedir( dir);

	sort_entries( dirfile);

	return TRUE;


abort:
	closedir( dir);

	if ( dirfile->nbr_icons > 0)
		free( dirfile->entry);
	return FALSE;
}


/*==================================================================================*
 * scan_mini_drv:																	*
 *		Creates a list with the available drives.									*
 *----------------------------------------------------------------------------------*
 * returns: TRUE if ok, FALSE if any error occured									*
 *==================================================================================*/
boolean scan_mini_drv( WINDOW *win)
{
	int 			current_drv, drv, allocated_entries = 0;
	uint32 			drives, tmp_d;
	Mini_Entry		*entry;
    WINDICON 		*dirlist = ( WINDICON *)DataSearch( win, WD_ICON);

	/* initialise directory descriptor */
	dirlist->nbr_child 		= 0;
	dirlist->entry 			= NULL;
	dirlist->mini_selected	= NULL;
	
	/* read contents of directory and build list */
	drives = Drvmap();

	tmp_d = drives;

	for ( current_drv = 0; current_drv < 32; current_drv++, tmp_d >>= 1)
	{
		if ( tmp_d & 0x1)
		{
			drv = 'A' + current_drv;
			
			if ( strchr( skip_drive, drv))
				continue;
		
			/* allocate new memory for entries if needed */
			if ( dirlist->nbr_child >= allocated_entries)
			{
				if ( dirlist->nbr_child == 0)
				{
					if (( dirlist->root = ( Mini_Entry *)malloc( ALLOC_NDIRENTRIES * sizeof( Mini_Entry))) == NULL)
						goto abort;
				}
				else
				{
					if (( entry = ( Mini_Entry *)realloc( dirlist->root, ( allocated_entries + ALLOC_NDIRENTRIES) * sizeof( Mini_Entry))) == NULL)
						goto abort;

					dirlist->root = entry;
				}
				allocated_entries += ALLOC_NDIRENTRIES;
			}
	
			/* fill out and add new entry */
			entry = &dirlist->root[dirlist->nbr_child];

			sprintf( entry->foldername, "%c:\\", drv);
			sprintf( entry->name, "%c", drv);

			entry->parent		= NULL;
			entry->child		= NULL;
			entry->nbr_child 	= 0;
			entry->icon_txt_w 	= get_text_width( entry->name);
			entry->state 		= UNKNOWN;

			dirlist->nbr_child++;
		}
	}

	return TRUE;

abort:
	if ( dirlist->nbr_child > 0)
		free( dirlist->root);
		
	errshow(NULL, -ENOMEM);
	
	return FALSE;
}


void check_mini_dir( int16 nbr_child, Mini_Entry *entry)
{
	int16			i, next;
	char 			old_dir[MAX_PATH];
	DIR	 			*dir;
	struct dirent	*de;
	struct stat		file_stat;

	for ( i = 0 ; i < nbr_child; i++)
	{			
		next = 0;

		if ( dir_cd( entry[i].foldername, old_dir, ( int)( sizeof old_dir)))
		{		
			if (( dir = opendir( entry[i].foldername)) != NULL)
			{
				while(( de = readdir( dir)) != NULL && next == 0)
				{
					if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
						continue;	
				
					if ( stat( de->d_name, &file_stat) != 0)
						continue;

					if ( S_ISDIR( file_stat.st_mode))
					{
						next = 1;
						entry[i].state = OFF;
					}
				}
				closedir( dir);
			}
			dir_cd( old_dir, NULL, 0);
		}
	}
}


boolean scan_mini_dir( WINDOW *win, Mini_Entry *parent)
{
	short 			allocated_entries = 0;
	char 			old_dir[MAX_PATH];
	Mini_Entry		*entry;
	DIR	 			*dir;
	struct dirent	*de;
	struct stat		file_stat;


	if ( dir_cd( parent->foldername, old_dir, ( int)( sizeof old_dir)))
	{
		if (( dir = opendir( ".")) != NULL)
		{
			while(( de = readdir( dir)) != NULL)
			{
				if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
					continue;	
		
				if ( stat( de->d_name, &file_stat) != 0)
					continue;

				if ( !S_ISDIR( file_stat.st_mode))
					continue;

				if (( file_stat.st_flags & 2) == FA_HIDDEN && !show_hidden)
					continue;

				if ( parent->nbr_child >= allocated_entries)
				{
					if ( parent->nbr_child == 0)
					{
						if (( parent->child = ( Mini_Entry *)malloc( ALLOC_NDIRENTRIES * sizeof( Mini_Entry))) == NULL)
							goto abort;
					}
					else
					{
						if (( entry = ( Mini_Entry *)realloc( parent->child, ( allocated_entries + ALLOC_NDIRENTRIES) * sizeof( Mini_Entry))) == NULL)
							goto abort;
			
						parent->child = entry;
					}
					allocated_entries += ALLOC_NDIRENTRIES;
				}

				entry = &parent->child[parent->nbr_child];
			
				strcpy( entry->foldername, parent->foldername);
				strcat( entry->foldername, de->d_name);
				strcat( entry->foldername, "\\");
				strcpy( entry->name, de->d_name);
	
				entry->parent		= parent;
				entry->child		= NULL;
				entry->nbr_child 	= 0;
				entry->icon_txt_w 	= get_text_width( entry->name);
				entry->state 		= UNKNOWN;

				parent->nbr_child++;
			}
			closedir( dir);
		}
		dir_cd( old_dir, NULL, 0);
	}

	qsort( parent->child, parent->nbr_child, sizeof( Mini_Entry), cmp_entries_name);

	return TRUE;


abort:
	closedir( dir);
	if ( parent->nbr_child > 0)
		free( parent->child);
	
	parent->nbr_child = 0;		

	return FALSE;
}


char *f_basename(const char *path)
{
	char *p1, *p2;

	p1 = strrchr(path, '/');
	p2 = strrchr(path, '\\');
	if (p1 == NULL || p2 > p1)
		p1 = p2;
	if (p1)
		return ++p1;
	return (char *)path;
}
