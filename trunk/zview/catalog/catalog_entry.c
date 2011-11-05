#include "../general.h"

/* Prototype */
void add_selected_entry( WINDICON *wicones, Entry *entry);
void remove_selected_entry( WINDICON *wicones, Entry *entry);
boolean check_selected_entry( WINDICON *wicones, Entry *entry);
void remove_all_selected_entry( WINDICON *wicones);
int16 return_entry_nbr( WINDICON *wicones, Entry *entry);


void add_selected_entry( WINDICON *wicones, Entry *entry)
{
	Entry *ptr = wicones->first_selected;
	
	if ( ptr == NULL)
		wicones->first_selected = entry;
	else
	{
		while( ptr->next_selected)
			ptr = ptr->next_selected;
			
		ptr->next_selected = entry;
	}
}


void remove_selected_entry( WINDICON *wicones, Entry *entry)
{
	Entry* ptr = wicones->first_selected;
	
	if ( ptr == entry) 
		wicones->first_selected = ptr->next_selected;
	else 
	{
		while ( ptr->next_selected) 
		{	
			if ( ptr->next_selected == entry)
				break;
			else
				ptr = ptr->next_selected;
		}	
	
		if ( ptr->next_selected == entry)
			ptr->next_selected = ptr->next_selected->next_selected;
	}
	
	entry->next_selected = NULL;
}


boolean check_selected_entry( WINDICON *wicones, Entry *entry)
{
	Entry* ptr = wicones->first_selected;

	if ( ptr == NULL)
		return FALSE;

	if ( ptr == entry)
		return TRUE;   
	
	while ( ptr->next_selected)
	{
		if ( ptr->next_selected == entry)
			return TRUE;
		else
			ptr = ptr->next_selected;		 
	}
	return FALSE;
}


void remove_all_selected_entry( WINDICON *wicones)
{
	while( wicones->first_selected)
		wicones->first_selected = wicones->first_selected->next_selected;
}


int16 return_entry_nbr( WINDICON *wicones, Entry *entry)
{
	int16 i;
	
	if ( entry == NULL)
		return ( -1);
		
	for ( i = 0 ; i < wicones->nbr_icons; i++)	 	 
		if ( entry == &wicones->entry[i])
			break;
	
	return i;		
}
