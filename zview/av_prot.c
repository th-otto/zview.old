#include "general.h"
#include "pic_load.h"
#include "winimg.h"

/* Prototype */
void va_start( void);

/* taken from mxPlay - simplified version, takes only one argument */
static char* ParseArgs( char* cmdline )
{
	BOOL	inQuote = FALSE;
	int		i = 0;
	int		j = 0;
	char	all[1023+1];

	while( cmdline[i] != '\0' )
	{
		if( cmdline[i] != '\'' && cmdline[i] != ' ' && inQuote == FALSE )
		{
			/* simple file (not in quotes) */
			while( cmdline[i] != '\0' && cmdline[i] != ' ' )
			{
				all[j++] = cmdline[i++];
			}

			all[j++] = '\0';
			strcpy( cmdline, all );
			return cmdline;
		}

		switch( cmdline[i] )
		{
			case '\'':
				i++;
				if( inQuote == TRUE )
				{
					if( cmdline[i] == '\'' )					/* '' -> ' */
					{
						all[j++] = '\'';
						i++;
					}
					else
					{
						inQuote = FALSE;
						all[j++] = '\0';
						strcpy( cmdline, all );
						return cmdline;
					}
				}
				else
				{
					inQuote = TRUE;
				}
				break;

			case ' ':
				if( inQuote == TRUE )
				{
					all[j++] = cmdline[i++];
				}
				else
				{
					i++;
				}
				break;

			default:
				all[j++] = cmdline[i++];
				break;

		}
	}

	return cmdline;
}


/*==================================================================================*
 * void va_start:																	*
 *		Handle the VA_START message from the server.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void va_start( void)
{
	char *p = *(char **) &evnt.buff[3];

	if( p)
	{
		p = ParseArgs( p );

		/* yes, I know, it's strange to reput the menubar here
		but if we don't do it, on MagiC, the menubar is not active after
		a VA_START :( */
		MenuBar( get_tree( MENU_BAR), 1);

		WindView( p);
		free( p);
	}
}
