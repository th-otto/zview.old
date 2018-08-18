/*----------------------------------------------------------------------*/
/*							Sortring Operations							*/
/*------------------- ---------------------------------------------------*/
#include "..//general.h"
#include "..//ztext.h"
#include "..//prefs.h"

/* prototype */
int cmp_entries_name( const void *obj_a, const void *obj_b);
static int cmp_entries_size( const void *obj_a, const void *obj_b);
static int cmp_entries_date( const void *obj_a, const void *obj_b);
static int cmp_entries_dirsbeforefiles(const void *obj_a, const void *obj_b);
int16 sort_entries( WINDICON *dirfile);


int cmp_entries_name(const void *obj_a, const void *obj_b)
{
	Entry *entry_a;
	Entry *entry_b;
	int cmp_val;
	char name_a[256];
	char name_b[256];

	entry_a = (Entry *)obj_a;
	entry_b = (Entry *)obj_b;

	strcpy(name_a, entry_a->name);
	str2lower(name_a);
	strcpy(name_b, entry_b->name);
	str2lower(name_b);

	cmp_val = strcmp(name_a, name_b);

	return cmp_val;
}


static int cmp_entries_size(const void *obj_a, const void *obj_b)
{
	Entry *entry_a;
	Entry *entry_b;
	int cmp_val;
	int32 size_a;
	int32 size_b;
	
	entry_a = (Entry *)obj_a;
	entry_b = (Entry *)obj_b;

	size_a = entry_a->stat.st_size;
	size_b = entry_b->stat.st_size;

	if ( size_a < size_b)
		cmp_val = -1;
	else if ( size_a == size_b)
		cmp_val = 0;
	else 
		cmp_val = 1;	    	

	return cmp_val;
}


static int cmp_entries_date(const void *obj_a, const void *obj_b)
{
	Entry *entry_a;
	Entry *entry_b;
	int cmp_val;
	int32 date_a;
	int32 date_b;
	
	entry_a = (Entry *)obj_a;
	entry_b = (Entry *)obj_b;

	date_a = entry_a->stat.st_mtime;
	date_b = entry_b->stat.st_mtime;

	if ( date_a < date_b)
		cmp_val = -1;
	else if ( date_a == date_b)
		cmp_val = 0;
	else 
		cmp_val = 1;	    	

	return cmp_val;
}


static int cmp_entries_dirsbeforefiles(const void *obj_a, const void *obj_b)
{
	Entry *entry_a;
	Entry *entry_b;
	int cmp_val;

	entry_a = (Entry *)obj_a;
	entry_b = (Entry *)obj_b;


	if ( entry_a->type == ET_DIR)
		cmp_val = ( ( entry_b->type == ET_DIR) ? 0 : -1);
	else 
		cmp_val = ( ( entry_b->type == ET_DIR) ?  1 :  0);

	return cmp_val;
} 


int16 sort_entries( WINDICON *dirfile)
{
	int16 nb_of_dirs = 0;

	if ( dirfile->nbr_icons < 2)
		return 1;

	switch ( sortingmode)
	{
		case 0:			/* directories before files */
			/* get number of directories */
			qsort(dirfile->entry, dirfile->nbr_icons, sizeof(Entry), cmp_entries_dirsbeforefiles);
			while ( (nb_of_dirs < dirfile->nbr_icons) && ( dirfile->entry[nb_of_dirs].type == ET_DIR))
				nb_of_dirs++;
			/* sort directories */
			if (nb_of_dirs > 1)
				qsort(dirfile->entry, nb_of_dirs, sizeof(Entry), cmp_entries_name);
			/* sort files */
			if (dirfile->nbr_icons - nb_of_dirs > 1)
				qsort(&dirfile->entry[nb_of_dirs], dirfile->nbr_icons - nb_of_dirs, sizeof(Entry), cmp_entries_name);
			break;

		case 1 :
			/* get number of directories */
			qsort(dirfile->entry, dirfile->nbr_icons, sizeof(Entry), cmp_entries_dirsbeforefiles);
			while ( (nb_of_dirs < dirfile->nbr_icons) && ( dirfile->entry[nb_of_dirs].type == ET_DIR))
				nb_of_dirs++;
			/* sort directories */
			if (nb_of_dirs > 1)
				qsort(dirfile->entry, nb_of_dirs, sizeof(Entry), cmp_entries_name);
			/* sort files */
			if (dirfile->nbr_icons - nb_of_dirs > 1)
				qsort(&dirfile->entry[nb_of_dirs], dirfile->nbr_icons - nb_of_dirs, sizeof(Entry), cmp_entries_size);
			break;

		case 2 :			/* files before directories */
			/* get number of directories */
			qsort(dirfile->entry, dirfile->nbr_icons, sizeof(Entry), cmp_entries_dirsbeforefiles);
			while ( (nb_of_dirs < dirfile->nbr_icons) && ( dirfile->entry[nb_of_dirs].type == ET_DIR))
				nb_of_dirs++;
			/* sort directories */
			if (nb_of_dirs > 1)
				qsort(dirfile->entry, nb_of_dirs, sizeof(Entry), cmp_entries_name);
			/* sort files */
			if (dirfile->nbr_icons - nb_of_dirs > 1)
				qsort(&dirfile->entry[nb_of_dirs], dirfile->nbr_icons - nb_of_dirs, sizeof(Entry), cmp_entries_date);
			break;
		
		default:
			break;
	}
	return 1;
}
