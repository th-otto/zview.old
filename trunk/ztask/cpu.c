#include "general.h"

char buf[2048];

extern uint32 total_stram;
extern uint32 total_ttram;



/* returns current cpu usage in percent */
inline int16 cpu_get_usage( void)
{
    static int32 pre_used = 0, pre_total = 0;
    int16 usage = 0;
    int32 mcpu, mnice, msystem, midle, used = 0, total = 0;
    FILE *fp;

    if ( !( fp = fopen("U:/kern/stat", "r"))) 
		return usage;

	fread( buf, 1, 2048, fp);	

    fclose(fp);

    sscanf( buf, "%*s %ld %ld %ld %ld", &mcpu, &mnice, &msystem, &midle);
    
    used  = mcpu + msystem + mnice;
    total = used + midle;

    /* calc CPU usage */
    if( total - pre_total > 0)
		usage = ( int16)(( 20 * ( double)( used - pre_used)) / ( double)( total - pre_total));

	if( usage < 0)
		usage = 0;

    pre_used = used;
    pre_total = total;

    return usage;
}


void get_total_ram( void)
{
    size_t bytes_read;
	char *match;
    FILE *fp;

   	total_stram = 0;
	total_ttram = 0;
    
    if ( !( fp = fopen("U:/kern/meminfo", "r"))) 
		return;

	bytes_read = fread( buf, 1, 2048, fp);
    
    fclose(fp);
    
    if( bytes_read == 0)
    	return;
    
    match = strstr( buf, "FastTotal");
    
    if( match == NULL)
    	return;
    
    sscanf( match, "FastTotal: %ld", &total_ttram);

    match = strstr( buf, "CoreTotal");
    
    if( match == NULL)
    	return;
    
    sscanf( match, "CoreTotal: %ld", &total_stram);
}


inline void get_free_ram( uint32 *st_ram, uint32 *tt_ram)
{
    size_t bytes_read;
    uint32 free_ram;
	char *match;
    FILE *fp;
    
    *tt_ram = 0;
    *st_ram = 0;        
    
    if ( !( fp = fopen("U:/kern/meminfo", "r"))) 
		return;

	bytes_read = fread( buf, 1, 2048, fp);
    
    fclose(fp);
    
    if( bytes_read == 0)
    	return;
    
    match = strstr( buf, "FastFree");
    
    if( match == NULL)
    	return;
    
    sscanf( match, "FastFree: %lu", &free_ram);

    *tt_ram = free_ram;
    
    match = strstr( buf, "CoreFree");
    
    if( match == NULL)
    	return;
    
    sscanf( match, "CoreFree: %lu", &free_ram);
    
    *st_ram = free_ram;    
}



inline void get_info_by_pid( int pid, char *name, char *ram_usage, uint32 *cpu_time)
{
    size_t bytes_read;
	char *s, *t;
	uint32 user_time, system_time;
	int32 ram;

    FILE *fp;
   
    sprintf( buf, "U:/kern/%d/stat", pid);

    if ( !( fp = fopen( buf, "r"))) 
		return;

	bytes_read = fread( buf, 1, 2048, fp);		

    fclose(fp);

    if( bytes_read == 0)
    	return;    
    
	s = strchr ( buf, '(') + 1;
	t = strchr ( buf, ')');
	strncpy( name, s, t - s);
	name[t - s] = '\0';    
    		
	sscanf( t + 2, "%*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*u %*d %*u %*u %ld", &user_time, &system_time, &ram);
	
	sprintf( ram_usage, "%ld kB", ram >> 10);

    *cpu_time = user_time + system_time;
}
