#include "general.h"

/* Prototype */
void mem_quit( void);
int16 mem_init( void);

LDG *ldg_mem = NULL;

/*==================================================================================*
 * int16 mem_init:																	*
 *		Initialise the memory manager.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      '1' if all is OK else '0'													*
 *==================================================================================*/

int16 mem_init( void)
{
	ldg_mem	= ldg_open( "C:\\gemsys\\ldg\\mem.ldg", ldg_global);

	if( !ldg_mem)
		ldg_mem = ldg_open( "mem.ldg", ldg_global);

	_ldg_malloc 	= ldg_find( "ldg_malloc", ldg_mem);
	_ldg_free 		= ldg_find( "ldg_free", ldg_mem);
	_ldg_allocmode 	= ldg_find( "ldg_allocmode", ldg_mem);

	if ( !ldg_mem || !_ldg_malloc || !_ldg_free || !_ldg_allocmode)
		return( 0);
	
	ldg_allocmode( 0L);

	return( 1);
}


/*==================================================================================*
 * void mem_quit:																	*
 *		close the memory manager.													*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void mem_quit( void)
{	
	if( ldg_mem)
		ldg_close( ldg_mem, ldg_global);
}

