#include "general.h"
#include "gmem.h"
#include "app.h"
#include "cpu.h"
#include "process.h"
#include <mint/cookie.h>

extern int16 cpu_history[100];
extern void main_win( void);

OBJECT *get_tree( int16 obj_index) 
{
	OBJECT *tree;
	RsrcGaddr( NULL, 0, obj_index, &tree);
	return tree;
}


char *get_string( int16 str_index) 
{
	char *txt;	
	rsrc_gaddr( 5, str_index,  &txt);
	return txt;
}


void applexit( WINDOW *w, short buff[8]) 
{
	WINDOW *last_closed = NULL;
	
    if( EvntFind( NULL, WM_XTIMER))
		EvntDelete( NULL, WM_XTIMER);

	/* Close all windows */
	while( wglb.first) 
	{
		if (last_closed != wglb.first) 
		{
			ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle, 0, 0, 0, 0);
			last_closed = wglb.first; /* to prevent sending toons of WM_CLOSED messages to each window */
		}
		
		if( EvntWindom( MU_MESAG | MU_TIMER) & MU_TIMER)  /* MU_TIMER event catched ? */
			last_closed = NULL; /* then WM_CLOSED message has been lost ! it should be resent */
	}

	prefs_write();

	while( root != NULL)
		app_data_delete( root->id); 

	while( process_root != NULL)
		process_delete( process_root->pid); 				

	RsrcXtype( 0, NULL, 0);
	RsrcFree();

	ApplExit();
	gMemReport();
	exit( 0);
}


int main( int argc, char *argv[])
{
	int16 i, dum, app_long_name;
	int32 mint = 0;
		
	ApplInit();

    if( app.nplanes < 4) 
	{
		( void)FormAlert( 1 , "[1][Sorry, zTask needs minimum a 16 colors display][Quit]");
		ApplExit();
		exit( 0);
	}
	
    if( Getcookie( C_MiNT, ( long*)&mint) != 0) 
	{
		( void)FormAlert( 1 , "[1][Sorry, zTask works only with MiNT][Quit]");
		ApplExit();
		exit( 0);
	}	

	mt_appl_getinfo( AES_EXTENDED, &dum, &dum, &app_long_name, &dum, app.aes_global);

	if( app_long_name <= 0) 
	{
		( void)FormAlert( 1 , "[1][AES 4.x needed!][Quit]");
		ApplExit();
		exit( 0);
	}	

	if( !RsrcLoad( "ztask.rsc")) 
	{
		( void)FormAlert( 1 , "[1][ztask.rsc not found!][Quit]");
		ApplExit();
		exit( 0);
	}	
		
	RsrcXtype( RSRC_XTYPE, NULL, 0);

	prefs_read();

	get_total_ram();

	evnt.timer 		= 20L;
	evnt.bclick   	= 258;
	evnt.bmask   	= 3;
	evnt.bstate   	= 0; 

	EvntAttach( NULL, AP_TERM, applexit);

	for( i = 0; i < 101;)
		cpu_history[i++] = 100;
	
	main_win();
	
	for(;;)
		EvntWindom( MU_MESAG|MU_TIMER|MU_KEYBD|MU_BUTTON);

	// applexit();
	return 0;
}

