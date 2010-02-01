
void *my_memcpy( void *dest, const void *source, unsigned long n)
{
	register unsigned char *dst, *src;
	register unsigned long count = n;

	dst  = ( unsigned char *)dest;
	src  = ( unsigned char *)source;

    if ( count == 0) return( dest);

    do 
	{
        *dst++ = *src++;
    } while ( --count != 0);

	return( dest);
}


int my_memcmp( const void *p1, const void *p2, unsigned long n)
{
    register int j;
	register unsigned char *s1, *s2;	
	register unsigned long count = n;

	s1  = ( unsigned char *)p1;
	s2  = ( unsigned char *)p2;

    for ( j = 0; j < count; j++) 
	{
        if ( s1[j] != s2[j]) 
			return 2 * ( s1[j] > s2[j]) - 1;
    }
    return 0;
}


void *my_memset( void *buf, int c, unsigned long n)
{
	register unsigned char *dst  = ( unsigned char *)buf;
	register unsigned char value = ( unsigned char)c;
	register unsigned long count = n;

    if ( count == 0) return( buf);

    do 
	{
        *dst++ = value;
    } while (--count != 0);

	return( buf);
}
