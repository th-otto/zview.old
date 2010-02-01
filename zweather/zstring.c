#include "general.h"

extern int vasprintf( int8 **strp, const int8 *fmt, va_list ap);


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

int8 *stpcpy( int8 *dest, const int8 *src)
{
	register int8 *d = dest;
	register const int8 *s = src;

	if( dest == NULL || src == NULL)
		return NULL;

	do
		*d++ = *s;
	while (*s++ != '\0');

	return d - 1;
}

int8 *strstr_len( const int8 *haystack, int32 haystack_len, const int8 *needle)
{
	if( haystack == NULL || needle == NULL)
		return NULL;
  
	if (haystack_len < 0)
		return strstr (haystack, needle);
	else
	{
		const int8 *end, *p = haystack;
		int32 i, needle_len = strlen ( needle);

		if (needle_len == 0)
			return (int8 *)haystack;

		if (haystack_len < needle_len)
			return NULL;
      
		end = haystack + haystack_len - needle_len;
      
		while (*p && p <= end)
		{
			for (i = 0; i < needle_len; i++)

			if (p[i] != needle[i])
				goto next;
	  
			return (int8 *)p;
	  
			next:
				p++;
		}
      
		return NULL;
	}
}


int8 *strdup_printf( const int8 *format, ...)
{
	int8	*buffer;
	int		len;
	va_list	args;

	va_start( args, format);

	len = vasprintf( &buffer, format, args);
	
	if( len < 0)
		buffer = NULL;

	va_end( args);

	return buffer;
}


int8* strconcat( const int8 *string1, ...)
{
	int32	l;     
	int8	*s, *concat, *ptr;
	va_list	args;

	if( !string1)
		return NULL;

	l = 1 + strlen( string1);
	va_start( args, string1);
	s = va_arg( args, int8*);

	while( s)
	{
		l += strlen (s);
		s = va_arg ( args, int8*);
    }
	va_end (args);

	concat = malloc( sizeof( int8) * l);
	ptr = concat;

	ptr = stpcpy ( ptr, string1);
	va_start (args, string1);
	s = va_arg (args, int8*);

	while (s)
	{
		ptr = stpcpy( ptr, s);
		s = va_arg (args, int8*);
    }
	va_end (args);

	return concat;
}

