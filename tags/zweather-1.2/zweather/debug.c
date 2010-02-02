#include "general.h"


/*==================================================================================*
 * void zdebug:																		*
 *		print debug information in 'wout' window.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		...	-> 	text and parameter are the same that sandard "printf" function.		*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void zdebug( const char *format, ...) 
{
#ifdef DEBUG
	va_list args;
	char fo_buff[255], *p;
	
	va_start( args, format);
	vsprintf( fo_buff, format, args);
	p = fo_buff;
	va_end( args);

	wout_printf("%s\n", p);
#endif	
}
