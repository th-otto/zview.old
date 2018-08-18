#include "general.h"
#include "close_modal.h"


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

void __CDECL close_modal( WINDOW *win EVNT_BUFF_PARAM)
{
	MenuEnable();
	frm_cls( win);
}
