#include "general.h"
#include "close_modal.h"
#include "scancode.h"


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


void __CDECL close_on_esc( WINDOW *win EVNT_BUFF_PARAM)
{
	switch (evnt.keybd >> 8)
	{
	case SC_ESC:
	case SC_UNDO:
		close_win(win);
		break;
	}
}


void close_win(WINDOW *win)
{
	ApplWrite(_AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}
