#include "general.h"

/* Prototype */
void close_modal( WINDOW *win); 


/*==================================================================================*
 * void close_modal:																*
 *		special destroy function for modal windows.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the modal window to destroy.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void close_modal( WINDOW *win) 
{
	MenuEnable();
	frm_cls( win);
}













