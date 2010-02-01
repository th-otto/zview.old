extern void inet_close (long fh);
extern short inet_host_addr (const char * name, long * addr);
extern long inet_connect (long addr, long port, long tout_sec);
extern long inet_send( long fh, const char * buf, size_t len);
extern long inet_recv (long fh, char *buf, size_t len);

