#include "general.h"
#include "transprt.h"

static boolean timeout = FALSE;
TPL *tpl = NULL;
DRV_LIST *drivers = NULL;


static void sig_alrm (long sig)
{
	(void)sig;
	timeout = TRUE;
}


void init_stik (void)
{
	if ( !tpl) 
	{
		struct {
			long cktag;
			long ckvalue;
		}  * jar = (void*)Setexc (0x5A0 /4, (void (*)())-1);
		long tag = 'STiK';
	
		while (jar->cktag) {
			if (jar->cktag == tag) {
				drivers = (DRV_LIST*)jar->ckvalue;
				if (strcmp (MAGIC, drivers->magic) == 0) {
					tpl = (TPL*)get_dftab(TRANSPORT_DRIVER);
				}
				break;
			}
			jar++;
		}
	}
}



void inet_close (long fh)
{
	if( fh >= 0) 
	{
		if ( tpl && use_sting) 
			TCP_close ((int16)fh, 0, NULL);
		else
			close (fh);
	}
}


short inet_host_addr ( const char * name, long * addr)
{
	short ret = -1;

	if ( tpl && use_sting) 
	{
	//	zdebug( "STING");
	
		if ( resolve ((char*)name, NULL, ( uint32*)addr, 1) > 0)
			ret = E_OK;
	}
	else
	{	
	//	zdebug( "MintNet");	

		struct hostent *host = gethostbyname (name);

		if (host) 
		{
			*addr = *(long*)host->h_addr;
			ret   = E_OK;
		} 
		else
			ret   = -errno;
	}	

	return ret;
}

long inet_connect (long addr, long port, long tout_sec)
{
	long fh = -1;

	if ( tpl && use_sting) 
	{
		long alrm = Psignal (14/*SIGALRM*/, (long)sig_alrm);
		if (alrm >= 0) {
			timeout = FALSE;
			Talarm (tout_sec);
		}
		if ((fh = TCP_open (addr, (int16)port, 0, 1024)) < 0) {
			fh = -( fh == -1001L ? ETIMEDOUT : 1);
		}
		if (alrm >= 0) {
			Talarm (0);
			Psignal (14/*SIGALRM*/, alrm);
		}
	}
	else
	{
		struct sockaddr_in s_in;
		s_in.sin_family = AF_INET;
		s_in.sin_port   = htons ((short)port);
		s_in.sin_addr   = *(struct in_addr *)&addr;
		if ((fh = socket( PF_INET, SOCK_STREAM, 0)) < 0) {
			fh = -errno;
		} else {
			long alrm = Psignal (14/*SIGALRM*/, (long)sig_alrm);	
			if (alrm >= 0) {
				timeout = FALSE;
				Talarm (tout_sec);
			}
			if (connect (fh, (struct sockaddr *)&s_in, sizeof (s_in)) < 0) {
				close (fh);
				fh = -( timeout && errno == EINTR ? ETIMEDOUT : errno);
				}
			if (alrm >= 0) {
				Talarm (0);
				Psignal (14/*SIGALRM*/, alrm);
			}
		}
	}
	
	return fh;
}


long inet_send( long fh, const char * buf, size_t len)
{
	long ret = -1;


	if ( tpl && use_sting) 
	{
		if ((ret = TCP_send ((int16)fh, (char*)buf, (int16)len)) == 0)
			ret = len;
	}
	else
		ret = Fwrite( fh, len, buf);

	return ret;
}

long inet_recv (long fh, char *buf, size_t len)
{
	long ret = 0;

	if ( tpl && use_sting) 
	{
		while (len) 
		{
			short n = CNbyte_count ((int16)fh);

			if (n < E_NODATA) 
			{
				if ( !ret) ret = (n == E_EOF || n == E_RRESET ? -316 : -1);
				break;
			} 
			else if (n > 0) 
			{
				if (n > len) n = len;
				if ((n = CNget_block ((int16)fh, buf, n)) < 0) 
				{
					if (!ret) ret = -1;
					break;
				} 
				else 
				{
					ret += n;
					buf += n;
					len -= n;
				}
			} else break;

		}
	}
	else
	{
		while (len) 
		{
			long n = Finstat (fh);
			if (n < 0) {
				if (!ret) ret = n;
				break;
			} else if (n == 0x7FFFFFFF) { /* connection closed */
				if ( !ret) ret = -316;
				break;
			} else if (n && (n = Fread (fh, ( n < len ? n : len), buf)) < 0) {
				if (!ret) ret = -errno;
				break;
			} else if (n) {
				ret += n;
				buf += n;
				len -= n;
			} else break;
		}
	}
	return ret;
}

