#include "general.h"

/* Prototype  */
void chrono_off( char *info);
void chrono_on( void);

/* Global variable */
int32 start_time, end_time;


/*==================================================================================*
 * void chrono_on:																	*
 *		save the current time in 'start_time' global variable.						*
 *----------------------------------------------------------------------------------*
 * input: 																			*
 *	   ---																			*
 *----------------------------------------------------------------------------------*
 * return: 																			*
 *	   ---																			*
 *==================================================================================*/

void chrono_on( void)
{
   	int32 oldssp = Super( 0L);
   	start_time = *( int32 *)0x4ba;
  	Super(( void *)oldssp);
}


/*==================================================================================*
 * void chrono_off:																	*
 *		save the current time in 'end_time' global variable and compute the time	*
 *		since the call of the 'chrono_on' function									*
 *----------------------------------------------------------------------------------*
 * input: 																			*
 *	   info ->	place where to write the time elapsed.								*
 *----------------------------------------------------------------------------------*
 * return: 																			*
 *	   ---																			*
 *==================================================================================*/
 
void chrono_off( char *info)
{
   	int32 oldssp = Super( 0L);
   	end_time = *( int32 *)0x4ba;
   	Super(( void *)oldssp);

	sprintf( info ,"%ld.%01ld sec", ( end_time - start_time) / 200L, (( end_time - start_time) % 200L) * 5L);
}
