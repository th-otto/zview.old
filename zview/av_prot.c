#include "general.h"
#include "pic_load.h"
#include "winimg.h"

/* Prototype */
void va_start( void);


/*==================================================================================*
 * void va_start:																	*
 *		Handle the VA_START message from the server.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
 
void va_start( void) 
{ 
	char *p = *(char **) &evnt.buff[3];

	if( p)
	{
		p = AvStrfmt( 1, p);
		
		/* yes, I know, it's strange to reput the menubar here
		but if we don't do it, on MagiC, the menubar is not active after
		a VA_START :( */
		MenuBar( get_tree( MENU_BAR), 1);

		WindView( p);
		free( p);
	}
}
