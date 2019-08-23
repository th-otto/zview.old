#ifndef __txt_data_defined
#define __txt_data_defined 1
#define MAX_TXT_DATA 256
typedef struct _txt_data
{
	int16_t		lines;					/* Number of lines in the text 						*/
	int16_t		max_lines_length;		/* The length of the greater line ( in nbr char)	*/
	char    	*txt[MAX_TXT_DATA];
} txt_data;
#endif
