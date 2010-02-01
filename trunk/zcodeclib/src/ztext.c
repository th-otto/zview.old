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
void str2lower(char *str);
void str2upper(char *str);


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

