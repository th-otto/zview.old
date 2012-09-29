#include "general.h"
#include "gmem.h"
#include "string.h"
#include "cpu.h"
#include <time.h>
#include <sys/time.h>

#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/vfs.h>

process *process_root = NULL;
int process_nbr = 0;


/*==================================================================================*
 * process_attach:																	*
 *		attach a entry in the global process list.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		pid:				The PID of the application.								*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      the processe created or NULL												*
 *==================================================================================*/

process *process_attach( int pid) 
{
	process *new = ( process *)gmalloc( sizeof(process));

	if( !new) 
		return NULL;

	new -> pid   				= pid;	
	new -> next 				= process_root;
	new->cpu_time				= 0;
		
	process_root = new;

	process_nbr++;
	
	return new;
}


/*==================================================================================*
 * process_find_parent:																*
 *		find the parent( previous) entry in the process list.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		child:			the function returns the parent of this entry.				*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      the parent entry else the root entry.										*
 *==================================================================================*/
process *process_find_parent( process *child) 
{
	process *scan = process_root;
	
	while( scan)
	{	
		if( scan->next != child)
		{
			scan = scan->next;
			continue;
		}	

		break;
	}	
	
	return( scan);
}


/*==================================================================================*
 * process_delete:																	*
 *		Delete an entry in the process list.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		pid:		the pid of the entry to delete.									*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void process_delete( int pid) 
{
	process *scan = process_root, *parent;
	
	while( scan)
	{
		if( scan->pid != pid)
		{
			scan = scan->next;
			continue;
		}	
		
		parent = process_find_parent( scan);
		
		if( parent == NULL)
			process_root = scan->next;
		else		
			parent->next = scan->next;

		gfree( scan); 
		process_nbr--;
		break;
	}	
}


/*==================================================================================*
 * process_find:																	*
 *		Find an entry in the process list.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		id:		the pid of the entry to find.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      the entry or NULL if not found.												*
 *==================================================================================*/
process *process_find( int pid) 
{
	process *scan = process_root, *result = NULL;
	
	while( scan)
	{	
		if( scan->pid != pid)
		{
			scan = scan->next;
			continue;
		}	
		
		result = scan;
		break;
	}	
	
	return( result);
}



/*==================================================================================*
 * scan_process:																	*
 *		Make a list of all the process and add it in the global process list if 	*
 *		necessary.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void scan_process( void)
{
	int 			pid, count = 0;
	uint32			old_cpu_time;
	DIR	 			*dir;
	char			*dirname;
	char			buf[128];	
	struct dirent	*de;
	process 		*current_process = NULL, *scan;
  	clock_t 		current_t, relative_t;
   	static 			clock_t old_t = 0;

	current_t = clock();
	
	relative_t = current_t - old_t;

	old_t = current_t;    
	
	if (( dir = opendir( "U:/kern")) == NULL)
		return;
		
	while(( de = readdir( dir)) != NULL)
	{
		dirname = de->d_name;

		if( dirname[0] != '1' && dirname[0] != '2' && dirname[0] != '3' && dirname[0] != '4' && dirname[0] != '5'
		 && dirname[0] != '6' && dirname[0] != '7' && dirname[0] != '8' && dirname[0] != '9')
			continue;
			
		count++;
		
		pid = atoi( dirname);
		
		current_process = process_find( pid);
		
		// Is already listed?
		if( current_process == NULL)
			current_process = process_attach( pid);

		old_cpu_time = current_process->cpu_time;

		get_info_by_pid( pid, current_process->name, current_process->ram_usage, &current_process->cpu_time);

		sprintf( current_process->cpu_usage, "%lu", ((current_process->cpu_time - old_cpu_time) * 20) / relative_t);
		
		current_process->ram_usage_txt_width = get_text_width( current_process->ram_usage);	
	}
	
	closedir( dir);
	
	if( count != process_nbr)
	{
		scan = process_root;
		
		while( scan != NULL)
		{
			sprintf( buf, "U:/kern/%d", scan->pid);
    
			if (( dir = opendir( buf)) == NULL)
			{
				pid	= scan->pid;
				scan = scan->next;
				process_delete( pid);
				continue;
			}
			else
				closedir( dir);
					
			scan = scan->next;
		}
	}
}



