#include "general.h"
typedef struct xtimer
{
	void (*func)( WINDOW *);
	WINDOW *win;
	struct xtimer *next;
} XTIMER;
static XTIMER *root = NULL;
static void std_xtimer( void) 
{
	XTIMER *scan;
	for( scan = root; scan; scan = scan->next) 
		(*scan->func)( scan->win);
}
void TimerInit( int32 slice_timer) 
{
	evnt.timer = slice_timer;
	EvntAttach( NULL, WM_XTIMER, std_xtimer);
}
int16 TimerAttach( WINDOW *win, void *func) 
{
	XTIMER *new = ( XTIMER *)gmalloc( sizeof(XTIMER));
	if( !new) 
		return FALSE;
	new -> win = win;
	new -> func = func;
	new -> next = root;
	root = new;
	return TRUE;
}
static XTIMER *find_parent_timer( XTIMER *child) 
{
	XTIMER *scan = root;
	
	while( scan)
	{	
		if( scan->next != child)
		{
			scan = scan->next;
			continue;
		}	
		break;
	}	
	
	return( scan);
}
void TimerDelete( WINDOW *win) 
{
	XTIMER *scan = root, *parent;
	
	while( scan)
	{	
		if( scan->win != win)
		{
			scan = scan->next;
			continue;
		}	
		parent = find_parent_timer( scan);
		
		if( parent == NULL)
			root = scan->next;
		else		
			parent->next = scan->next;
		gfree( scan); 
		break;		
	}	
}
