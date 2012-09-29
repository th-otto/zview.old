#include "general.h"

static void quit_dialog_cancel_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
}

static void quit_dialog_ok_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
	ApplWrite( _AESapid, AP_TERM, 0, 0, 0, 0, 0);
}


void quit_dialog( void)
{
	WINDOW 	*win;

	if( ( win = FormCreate( get_tree( QUIT), NAME|MOVER, NULL, "", NULL, TRUE, FALSE)) == NULL)
		return;

	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	ObjcAttachFormFunc( win, QUIT_YES, quit_dialog_ok_event, NULL);
	ObjcAttachFormFunc( win, QUIT_NO, quit_dialog_cancel_event, NULL);
}
