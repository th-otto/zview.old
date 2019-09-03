#include "general.h"
#include "pic_load.h"
#include "winimg.h"
#include "av_prot.h"


/* taken from mxPlay - simplified version, takes only one argument */
static char *ParseArgs(const char *cmdline, char *all)
{
	int inQuote = FALSE;
	int i = 0;
	int j = 0;

	if (cmdline == NULL)
		return NULL;
	while (cmdline[i] != '\0' && j < 1024)
	{
		if (cmdline[i] != '\'' && cmdline[i] != ' ' && inQuote == FALSE)
		{
			/* simple file (not in quotes) */
			while (cmdline[i] != '\0' && cmdline[i] != ' ' && j < 1024)
			{
				all[j++] = cmdline[i++];
			}
			break;
		}

		switch (cmdline[i])
		{
		case '\'':
			i++;
			if (inQuote)
			{
				if (cmdline[i] == '\'')	/* '' -> ' */
				{
					all[j++] = '\'';
					i++;
				} else
				{
					inQuote = FALSE;
					all[j] = '\0';
					return all;
				}
			} else
			{
				inQuote = TRUE;
			}
			break;

		case ' ':
			if (inQuote)
			{
				all[j++] = cmdline[i++];
			} else
			{
				i++;
			}
			break;

		default:
			all[j++] = cmdline[i++];
			break;
		}
	}

	all[j] = '\0';
	return all;
}


/*==================================================================================*
 * void va_protokoll_start:															*
 *		Handle the VA_START message from the server.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void __CDECL va_protokoll_start(WINDOW * win EVNT_BUFF_PARAM)
{
	const char *const *pp = (const char *const *) &EVNT_BUFF[3];
	const char *p = *pp;
	char all[1023 + 1];

	p = ParseArgs(p, all);

	/* yes, I know, it's strange to reput the menubar here
	   but if we don't do it, on MagiC, the menubar is not active after
	   a VA_START :( */
	MenuBar(get_tree(MENU_BAR), 1);

	if (p && *p)
		WindViewPath(p);
}
