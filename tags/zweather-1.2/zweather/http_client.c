#include "general.h"
#include "inet.h"

long http_recv( int32 fd, int8 **buffer)
{
	long n = 0L; /* 1 = good, 0 = conn terminated, -1 = error */
	int8 thisbuffer[1024]; 
	clock_t time_out = 0;

	for(;;)
	{
		n = inet_recv ( fd, thisbuffer, 1023L);

		if( n < 0L)
		{
			*buffer = NULL;
			break;
		}
		else if( n == 0)
		{
			clock_t clk = clock();
	
			if ( !time_out) 
			{
				time_out = clk;
				continue;
			}
			else if (( clk -= time_out) < 3000) 
			{
				continue;
			}
			else break;
		}
	    else
		{
			thisbuffer[n] = '\0';
			*buffer = strdup(( const int8 *)thisbuffer); 
			break;
		}
	}
	return n;
}


boolean http_get_header( int32 fd, int8 **buffer)
{
	int8 lastchar = 0, *thisbuffer;
	int32 l;

	while((l = http_recv( fd, &thisbuffer)) > 0)
	{
		boolean found = FALSE;
		int8 *where = NULL, *p;

		if (lastchar == '\r' && ( p = strstr_len( thisbuffer, 3, "\n\r\n")))
		{
			where = p + 3;
			found = TRUE;
		}
		else if (( p = strstr(thisbuffer, "\r\n\r\n"))) 
		{
			where = p + 4;
			found = TRUE;
		}

		if( found)
		{
			/*TODO check if at end*/
			*buffer = strdup(where);
		}
		else
			lastchar = thisbuffer[l];

		free(thisbuffer);

		if( found) 
			return TRUE;
	}

	return FALSE;
}


int16 http_get_file( int8 *url, int8 *hostname, int8 *filename)
{
	int32 addr, fd = -1, error;
	FILE *file = NULL;
	int8 *buffer= NULL, *request = NULL;

	if( inet_host_addr ( hostname, &addr) != E_OK)
	{ 
		return FALSE;
	}

	if( !( fd = inet_connect( addr, 80, 10)))
	{ 
		return FALSE;
	}

	request = strdup_printf( "GET %s HTTP/1.0\r\n""HOST: %s\r\n\r\n", url, hostname);

	if( request == NULL)
	{
		inet_close(fd);
		return FALSE;
	}

	error = inet_send( fd, request, strlen(request));
	free( request);

	if ( error <= 0)
	{ 
		inet_close(fd);
		return FALSE;
	}

	file = fopen( filename, "w");

	if ( !file)
	{ 
		inet_close(fd);
		return FALSE;
	}


	if( http_get_header( fd, &buffer) == FALSE)
	{
		inet_close(fd);
		fclose(file);
		return FALSE;
	}


	if( buffer)
	{
		int l = strlen(buffer);
		fwrite(buffer, sizeof(char), l, file);
		free(buffer);
	}

	while(( error = http_recv(fd, &buffer)) > 0)
	{
		int l = strlen( buffer);
		fwrite( buffer, sizeof(char), l, file);
		free(buffer);
	}

	fclose(file);
	inet_close(fd);

	if( error != -316)
		return FALSE;

	return TRUE;
}
