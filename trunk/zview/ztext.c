#include "general.h"


/* Local variable */
typedef struct 
{
	char upper;
	char lower;
} CUL;


static CUL c[] = { 	{'A', 'a'}, {'B', 'b'}, {'C', 'c'}, {'D', 'd'}, {'E', 'e'}, {'F', 'f'}, {'G', 'g'}, {'H', 'h'},
					{'I', 'i'}, {'J', 'j'}, {'K', 'k'}, {'L', 'l'}, {'M', 'm'}, {'N', 'n'}, {'O', 'o'}, {'P', 'p'},
				 	{'Q', 'q'}, {'R', 'r'}, {'S', 's'}, {'T', 't'}, {'U', 'u'}, {'V', 'v'}, {'W', 'w'}, {'X', 'x'},
					{'Y', 'y'}, {'Z', 'z'}, {'ö', 'Å'}, {'ê', 'Ç'}, {'é', 'Ñ'}, {0xb6, 'Ö'}, {'è', 'Ü'}, {'Ä', 'á'},
					{'í', 'ë'}, {'ô', 'î'}, {'•', '§'}, {0xb7, 0xb0}, {0xb8, 0xb1}, {0xb2, 0xb3}, {0xb5, 0xb4} };


/* Prototype */
void size_to_text( char *txt, float size);
void str2lower(char *str);
void str2upper(char *str);
int16 name_shorter( int16 handle, int16 max_size, char *str);
char *zstrncpy( char *dst, const char *src, size_t n);


/*==================================================================================*
 * zstrncpy:																		*
 * 		Copy <n> bytes from <src> to <dst>, including the terminating \0'-byte.		* 																	*
 *		If <src> is longer than <n-1>, it will be truncated with a '\0'-byte at		* 
 * 		position n-1.																* 
 *		Example: zstrncpy(b, "string", 1) would put a '\0'-byte at b[0] and 		*
 *		return. 																	*
 *		Basically, think of the <n> as a limiter on the number of bytes at <dst> 	*
 *		we're allowed to touch, and	then fit a string-copying idea on top of that.	*
 *		Works for me, and a	lot better than the standard strncpy(), too.			*
 *----------------------------------------------------------------------------------*
 * returns: <dst>																	*
 *----------------------------------------------------------------------------------*
 * 	dst:			Pointer to destination text.									*
 * 	src:			Pointer to source text.											*
 *	n:				Limiter on the number of bytes at <dst>							*
 *==================================================================================*/

char *zstrncpy( char *dst, const char *src, size_t n)
{
	register const char *source 	= src;
	register char *dest			 	= dst;
	register size_t count		 	= n;

	if( dest && !source && count)
		*dest = '\0';
	else if( dest && source && (count >= 1))
	{
		for( count--; count > 0 && *source != '\0'; count--)
			*dest++ = *source++;
		*dest = '\0';
	}
	return dst;
}

/*==================================================================================*
 * fullpathname_to_filename:														*
 *		return a file name from a full path name.									*
 *----------------------------------------------------------------------------------*
 * returns: the filename															*
 *----------------------------------------------------------------------------------*
 * 	fullpathname:	the full path name.												*
 *==================================================================================*/

char *fullpathname_to_filename( char *fullpathname)
{
		int16			path_len, file_len;
		int8			*fullname = fullpathname; 

		file_len = ( int16)strlen( fullname);
		path_len = file_len;

		while(( fullname[path_len] != '/') && ( fullname[path_len] != '\\'))
			path_len--;

		path_len++;
		
		return &fullname[path_len];
}

/*==================================================================================*
 * size_to_text:																	*
 *		convert <size> to a Human readable string.									*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *----------------------------------------------------------------------------------*
 * 	txt:			Pointer to a text for the convertion.							*
 * 	size:			File size in byte.												*
 *==================================================================================*/

void size_to_text( char *txt, float size)
{
	char format[10];

	if ( size > 1023.0)
	{
		size = size / 1024.0; 			

		if ( size > 1023.0)
		{
			size = size / 1024.0; 			
			sprintf( format, "MB");
		}
		else
			sprintf( format, "KB");
		
		sprintf( txt, "%.1f %s", size, format);			   
	}
	else
		sprintf( txt, "%.0f bytes", size);		   
}

/*==================================================================================*
 * str2lower:																		*
 * 		Transforms all letters in the string to lower case.							*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *----------------------------------------------------------------------------------*
 * 	str:			Pointer to null terminated string.								*
 *==================================================================================*/
void str2lower(char *str)
{
	int16 i, k;

	i = 0;
	while (str[i] != '\0')
	{
		for (k = 0; k < (sizeof(c) / sizeof(CUL)); k++)
		{
			if (c[k].upper == str[i])
			{
				str[i] = c[k].lower;
				break;
			}
		}
		i++;
	}
}


/*==================================================================================*
 * str2upper:																		*
 * 		Transforms all letters in the string to uper case.							*
 *----------------------------------------------------------------------------------*
 * returns: -																		*
 *----------------------------------------------------------------------------------*
 * 	str:			Pointer to null terminated string.								*
 *==================================================================================*/
void str2upper(char *str)
{
	int16 i, k;

	i = 0;
	while (str[i] != '\0')
	{
		for (k = 0; k < (sizeof(c) / sizeof(CUL)); k++)
		{
			if (c[k].lower == str[i])
			{
				str[i] = c[k].upper;
				break;
			}
		}
		i++;
	}
}

