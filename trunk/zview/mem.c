#include "general.h"

/* Prototype */
void mem_quit( void);
void mem_init( void);

/* Global variable */
LDG *ldg_mem = NULL;

/*==================================================================================*
 * void mem_init:																	*
 *		Initialise the memory manager.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void mem_init( void)
{
	ldg_mem = ldg_open( "mem.ldg", ldg_global);

	if( !ldg_mem)
	{
		ldg_mem	= ldg_open( "C:\\gemsys\\ldg\\mem.ldg", ldg_global);

		if( !ldg_mem)
		{
			errshow( "MEM.LDG", ldg_error());
			applexit();
		}
	}

	_ldg_malloc 	= ldg_find( "ldg_malloc", ldg_mem);
	_ldg_realloc	= ldg_find( "ldg_realloc", ldg_mem);
	_ldg_free 		= ldg_find( "ldg_free", ldg_mem);	
	_ldg_allocmode 	= ldg_find( "ldg_allocmode", ldg_mem);

	if ( !_ldg_malloc || !_ldg_realloc || !_ldg_free || !_ldg_allocmode)
	{
		errshow( "MEM.LDG", ldg_error());
		applexit();
	}
	
	ldg_allocmode( 0L);		
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


