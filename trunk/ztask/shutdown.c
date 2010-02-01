#include "general.h"

static int shutdown_mode = SHUTDOWN_DIAL_HALT;


static void shutdown_dialog_cancel_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
}

static void shutdown_dialog_ok_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
	Shutdown(( long)shutdown_mode);
}


void shutdown_dialog( void)
{
	WINDOW 	*win;

	if( ( win = FormCreate( get_tree( SHUTDOWN_DIAL), NAME|MOVER, NULL, get_string( S_SHUTDOWN), NULL, TRUE, FALSE)) == NULL)
		return;

	ObjcAttachVar( OC_FORM, win, SHUTDOWN_DIAL_WARN, &shutdown_mode, 1);
	ObjcAttachVar( OC_FORM, win, SHUTDOWN_DIAL_COLD, &shutdown_mode, 2);
	ObjcAttachVar( OC_FORM, win, SHUTDOWN_DIAL_HALT, &shutdown_mode, 0);

	ObjcAttachFormFunc( win, SHUTDOWN_DIAL_OK, shutdown_dialog_ok_event, NULL);
	ObjcAttachFormFunc( win, SHUTDOWN_DIAL_CANCEL, shutdown_dialog_cancel_event, NULL);
}

